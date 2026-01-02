import asyncio

from fastapi import APIRouter, Depends, Response
from dependency_injector.wiring import inject, Provide
from clear_vision.config.logger import get_logger
from clear_vision.domain.services import InferenceService
from clear_vision.entrypoints.api.api_models import AddInferenceResponse, BaseResponse
from clear_vision.interfaces.repositories import (
    InferenceRepositoryInterface,
    VideoRepositoryInterface,
)
from clear_vision.interfaces.detectors import GeneralTargetDetectorInterface

inference_router = APIRouter(prefix="/clear-vision/v1")

logger = get_logger(__name__)


@inference_router.post("/inferences/")
@inject
async def add_inference(
        video_uid: str,
        target: str,
        response: Response,
        detector: GeneralTargetDetectorInterface = Depends(Provide["detector"]),
        inference_repository: InferenceRepositoryInterface = Depends(
            Provide["inference_repository"]
        ),
        video_repository: VideoRepositoryInterface = Depends(Provide["video_repository"]),
):
    logger.info(f'Target "{target}" requested on video {video_uid}')

    try:
        service = InferenceService(
            inference_repository=inference_repository,
            video_repository=video_repository,
        )

        # Move to another thread so it wont block future requests
        inference = await asyncio.to_thread(
            service.add_inference,
            video_uid=video_uid,
            target=target,
            detector=detector,
        )
        response.status_code = 200

        return AddInferenceResponse(
            message="Inference completed",
            uid=inference.uid,
            video_uid=inference.video_uid
        )
    except Exception as e:
        message = f"Error when adding inference: {e}"
        logger.exception(message)
        response.status_code = 500
        return BaseResponse(message=message)
