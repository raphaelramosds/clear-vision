import asyncio
import os
from fastapi import APIRouter, Depends, Form, UploadFile, WebSocket, WebSocketDisconnect
from tempfile import NamedTemporaryFile
from dependency_injector.wiring import inject, Provide

from clear_vision.domain.entities import Video
from clear_vision.domain.services import VideoService
from clear_vision.interfaces.repositories import VideoRepositoryInterface

videos_router = APIRouter(prefix="/clear-vision/v1")

processing_tasks = {}


@videos_router.post("/upload/")
@inject
async def upload_video(
    video: UploadFile,
    video_repository: VideoRepositoryInterface = Depends(Provide["video_repository"]),
):
    dot_ext = os.path.splitext(video.filename)[1]
    content = await video.read()

    temp = NamedTemporaryFile(delete=False, suffix=dot_ext)
    temp.write(content)
    temp.flush()

    service = VideoService(video_repository=video_repository)
    model = service.add_video(video_path=temp.name)

    return {
        "message": "File uploaded",
        "file": model.model_dump(),
    }


# @inject
# async def run_video_target_detection(
#     job_id: str,
#     path: str,
#     target: str,
#     frame_sampler: FrameSamplerInterface = Depends(Provide["frame_sampler"]),
#     detector: GeneralTargetDetectorInterface = Depends(Provide["detector"]),
# ):
#     try:
#         # inference_service = InferenceService(
#         #     detector=detector, frame_sampler=frame_sampler
#         # )

#         processing_tasks[job_id] = {
#             "status": "done",
#             # "result": inference_service.run_video_target_detection(
#             #     video_path=path, target=target
#             # ),
#             "result": "pronto",
#         }

#     except Exception as e:
#         processing_tasks[job_id] = {"status": "error", "error": str(e)}
#     finally:
#         pass


# @videos_router.websocket("/ws/{job_id}")
# async def ws_job_status(websocket: WebSocket, job_id: str):
#     await websocket.accept()
#     try:
#         while True:
#             await asyncio.sleep(1)
#             task_info = processing_tasks.get(job_id)
#             if not task_info:
#                 await websocket.send_json({"status": "not_found"})
#                 break

#             if task_info["status"] == "done":
#                 await websocket.send_json(
#                     {"status": "done", "result": task_info.model_dump()}
#                 )
#                 break
#             elif task_info["status"] == "error":
#                 await websocket.send_json(
#                     {
#                         "status": "error",
#                         "message": task_info.get("error"),
#                     }
#                 )
#                 break
#             else:
#                 await websocket.send_json({"status": "processing"})
#     except WebSocketDisconnect:
#         print(f"Cliente desconectado do job {job_id}")
