import typing as t
from fastapi import APIRouter, Depends
from dependency_injector.wiring import inject, Provide

from clear_vision.domain.services import InferenceService, VideoService
from clear_vision.interfaces.repositories import (
    InferenceRepositoryInterface,
    VideoRepositoryInterface,
)
from clear_vision.interfaces.detectors import GeneralTargetDetectorInterface
from clear_vision.interfaces.frame_samplers import FrameSamplerInterface

inference_router = APIRouter(prefix="/clear-vision/v1")


@inference_router.post("/inferences/")
@inject
def add_inference(
    video_uid: str,
    target: str,
    detector: GeneralTargetDetectorInterface = Depends(
        Provide["detector"]
    ),
    frame_sampler: FrameSamplerInterface = Depends(Provide["frame_sampler"]),
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

    inference = service.add_inference(
        video_uid=video_uid,
        target=target,
        detector=detector,
        frame_sampler=frame_sampler,
    )

    return inference.model_dump()


@inference_router.get("/inferences/{inference_uid}")
@inject
def get_inference(
    inference_uid: str,
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

    inference = service.get_inference(inference_uid)

    return inference.model_dump()
