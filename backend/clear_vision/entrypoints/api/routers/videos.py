import asyncio
import uuid
from os import path as os_path
from typing import Annotated
from fastapi import APIRouter, Depends, Form, UploadFile, WebSocket, WebSocketDisconnect
from tempfile import NamedTemporaryFile
from dependency_injector.wiring import inject, Provide

videos_router = APIRouter(prefix="/clear-vision/v1")

processing_tasks = {}


@videos_router.post("/upload/")
@inject
async def upload(
    video: UploadFile,
    user_prompt: Annotated[str, Form()],
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
        )
    )
    processing_tasks[job_id] = {"status": "processing", "task": task}

    return {"job_id": job_id, "message": "Upload recebido. Processando em background."}


async def process_video(job_id: str, path: str, user_prompt: str):
    try:
        response = None # TODO
        processing_tasks[job_id] = {
            "status": "done",
            "result": response,
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
                        # "result" : "something"
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
