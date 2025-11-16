import asyncio
import uuid
from os import path as os_path
from typing import Annotated
from fastapi import APIRouter, Depends, Form, UploadFile, WebSocket, WebSocketDisconnect
from tempfile import NamedTemporaryFile
from dependency_injector.wiring import inject, Provide
from langchain_ollama import OllamaLLM

from clear_vision.domain.command import SendMessageCommand
from clear_vision.interfaces.frame_samplers import FrameSamplerInterface
from clear_vision.use_cases.send_message import SendMessageUseCase

videos_router = APIRouter(prefix="/clear-vision/v1")

processing_tasks = {}


@videos_router.post("/upload/")
@inject
async def upload(
    video: UploadFile,
    user_prompt: Annotated[str, Form()],
    frame_sampler: FrameSamplerInterface = Depends(Provide["frame_sampler"]),
):
    dot_ext = os_path.splitext(video.filename)[1]
    content = await video.read()
    size_mb = len(content) / (1024 * 1024)

    temp = NamedTemporaryFile(delete=False, suffix=dot_ext)
    temp.write(content)
    temp.flush()

    job_id = str(uuid.uuid4())

    task = asyncio.create_task(
        process_video(
            job_id=job_id,
            path=temp.name,
            user_prompt=user_prompt,
            frame_sampler=frame_sampler,
        )
    )
    processing_tasks[job_id] = {"status": "processing", "task": task}

    return {"job_id": job_id, "message": "Upload recebido. Processando em background."}


async def process_video(job_id: str, path: str, user_prompt: str, frame_sampler):
    try:
        send_message_use_case = SendMessageUseCase()
        command = SendMessageCommand(
            path_to_video=path,
            user_prompt=user_prompt,
        )
        num_frames = send_message_use_case(
            command=command,
            frame_sampler=frame_sampler,
        )

        context = f"""
        O vídeo '{os_path.basename(path)}' contém {num_frames} frames.
        Pergunta do usuário: {user_prompt}.
        Gere uma resposta detalhada e natural com base nas informações do vídeo.
        """

        llm = OllamaLLM(model="qwen3:1.7b", base_url="http://ollama-server:11434")
        response = llm.invoke(context)

        processing_tasks[job_id] = {
            "status": "done",
            "result": response,
            "num_frames": num_frames,
        }

    except Exception as e:
        processing_tasks[job_id] = {"status": "error", "error": str(e)}
    finally:
        pass


@videos_router.websocket("/ws/{job_id}")
async def ws_job_status(websocket: WebSocket, job_id: str):
    await websocket.accept()
    try:
        while True:
            await asyncio.sleep(1)
            task_info = processing_tasks.get(job_id)
            if not task_info:
                await websocket.send_json({"status": "not_found"})
                break

            if task_info["status"] == "done":
                await websocket.send_json(
                    {
                        "status": "done",
                        "num_frames": task_info.get("num_frames"),
                        "result": task_info.get("result"),
                    }
                )
                break
            elif task_info["status"] == "error":
                await websocket.send_json(
                    {
                        "status": "error",
                        "message": task_info.get("error"),
                    }
                )
                break
            else:
                await websocket.send_json({"status": "processing"})
    except WebSocketDisconnect:
        print(f"Cliente desconectado do job {job_id}")
