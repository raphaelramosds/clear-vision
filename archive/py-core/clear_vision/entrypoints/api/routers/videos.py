import os
from fastapi import APIRouter, Depends, Response, UploadFile
from tempfile import NamedTemporaryFile
from dependency_injector.wiring import inject, Provide

from clear_vision.config.logger import get_logger
from clear_vision.domain.exceptions import GeneralError, EmptyRepositoryError
from clear_vision.domain.services import InferenceService, VideoService
from clear_vision.entrypoints.api.api_models import BaseResponse, GetVideosResponse, \
    GetVideoInferencesResponse, UploadVideoResponse, GetVideoResponse
from clear_vision.interfaces.frame_samplers import FrameSamplerInterface
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
        response: Response,
        video_repository: VideoRepositoryInterface = Depends(Provide["video_repository"]),
):
    try:
        # Download on local file system
        dot_ext = os.path.splitext(video.filename)[1]
        content = await video.read()

        temp = NamedTemporaryFile(delete=False, suffix=dot_ext)
        temp.write(content)
        temp.flush()

        # Add reference to this video on repository
        service = VideoService(video_repository=video_repository)
        video = service.add_video(
            video_path=temp.name,
        )

        response.status_code = 200
        return UploadVideoResponse(
            message="File uploaded",
            content=video,
        )
    except GeneralError as e:
        response.status_code = 400
        return BaseResponse(
            message=f"An error occurred: {e}",
        )
    except Exception as e:
        logger.error(f"Unexpected error: {e}")
        response.status_code = 500
        return BaseResponse(
            message=f"An unexpected error occurred while uploading video"
        )


@videos_router.get("/videos")
@inject
def get_videos(
        response: Response,
        video_repository: VideoRepositoryInterface = Depends(Provide["video_repository"]),
        frame_sampler: FrameSamplerInterface = Depends(Provide["frame_sampler"]),
):
    try:
        service = VideoService(video_repository=video_repository)
        videos = service.get_videos_with_thumbnail(frame_sampler=frame_sampler)
        response.status_code = 201
        return GetVideosResponse(message="Videos fetched successfully", content=videos)
    except EmptyRepositoryError as e:
        logger.warning(f"Video repository is empty: {e}")
        response.status_code = 200
        return GetVideosResponse(
            message="Video repository is empty",
            content=[],
        )
    except Exception as e:
        logger.error(f"Unexpected error: {e}")
        response.status_code = 500
        return BaseResponse(
            message=f"An unexpected error occurred while fetching videos"
        )


@videos_router.get("/videos/{video_uid}")
@inject
def get_video(
        response: Response,
        video_uid: str,
        video_repository: VideoRepositoryInterface = Depends(Provide["video_repository"]),
        frame_sampler: FrameSamplerInterface = Depends(Provide["frame_sampler"]),
):
    try:
        service = VideoService(video_repository=video_repository)
        video = service.get_video_with_thumbnail(video_uid=video_uid, frame_sampler=frame_sampler)
        return GetVideoResponse(
            message="Video fetched successfully",
            content=video
        )
    except GeneralError as e:
        logger.error(f"Something went wrong when get this video: {e}")
        response.status_code = 400
        return BaseResponse(message=f"Something wen wrong, sorry.")
    except Exception as e:
        logger.error(f"Unexpected error: {e}")
        response.status_code = 500
        return BaseResponse(
            message=f"An unexpected error occurred while fetching videos"
        )


@videos_router.get("/videos/{video_uid}/inferences")
@inject
def get_video_inferences(
        video_uid: str,
        response: Response,
        inference_repository: InferenceRepositoryInterface = Depends(
            Provide["inference_repository"]
        ),
        video_repository: VideoRepositoryInterface = Depends(Provide["video_repository"]),
):
    try:
        service = InferenceService(
            inference_repository=inference_repository,
            video_repository=video_repository,
        )
        inferences_simplified = service.get_video_inferences_simplified(video_uid)
        response.status_code = 200
        return GetVideoInferencesResponse(
            message="Inferences fetched successfully",
            content=inferences_simplified,
        )
    except GeneralError as e:
        logger.error(f"Something went wrong when get this video inferences: {e}")
        response.status_code = 400
        return BaseResponse(message=f"Something wen wrong, sorry.")
    except Exception as e:
        logger.error(f"Unexpected error: {e}")
        response.status_code = 500
        return BaseResponse(
            message=f"An unexpected error occurred while fetching videos inferences"
        )