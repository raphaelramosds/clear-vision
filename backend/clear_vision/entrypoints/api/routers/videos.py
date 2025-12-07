from math import e
import os
from fastapi import APIRouter, Depends, Response, UploadFile
from tempfile import NamedTemporaryFile
from dependency_injector.wiring import inject, Provide

from clear_vision.config.logger import get_logger
from clear_vision.domain.services import InferenceService, VideoService
from clear_vision.entrypoints.api.api_models import BaseResponse, GetVideosResponse
from clear_vision.interfaces.repositories import (
    InferenceRepositoryInterface,
    VideoRepositoryInterface,
)

videos_router = APIRouter(prefix="/clear-vision/v1")

logger = get_logger(__name__)


@videos_router.post("/videos/upload/")
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


@videos_router.get("/videos")
@inject
def get_videos(
    response: Response,
    video_repository: VideoRepositoryInterface = Depends(Provide["video_repository"])
):
    try:
        service = VideoService(video_repository=video_repository)
        videos = service.get_videos()
        response.status_code = 201
        return GetVideosResponse(
            message="Videos fetched successfully",
            content=videos
        )
    except Exception as e:
        logger.error(f'Unexpected error: {e}')
        response.status_code = 500
        return BaseResponse(
            message=f"An unexpected error occurred while fetching videos"
        )

@videos_router.get("/videos/{video_uid}")
@inject
def get_video(
    video_uid: str,
    video_repository: VideoRepositoryInterface = Depends(Provide["video_repository"]),
):
    service = VideoService(video_repository=video_repository)
    video = service.get_video(video_uid)

    return video.model_dump()


@videos_router.get("/videos/{video_uid}/inferences")
@inject
def get_video_inferences(
    video_uid: str,
    inference_repository: InferenceRepositoryInterface = Depends(
        Provide["inference_repository"]
    ),
    video_repository: VideoRepositoryInterface = Depends(Provide["video_repository"]),
):
    video_service = VideoService(video_repository=video_repository)

    service = InferenceService(
        inference_repository=inference_repository,
        video_service=video_service,
    )

    inferences = service.get_video_inferences(video_uid)

    return [inf.model_dump() for inf in inferences]
