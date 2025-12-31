from dependency_injector import containers, providers

from clear_vision.adapters.detectors import LlavaTargetDetector
from clear_vision.adapters.frame_samplers import Cv2FullVideoFrameSampler
from clear_vision.adapters.repositories.dynamodb import (
    InferenceDynamoDbRepository,
    VideoDynamoDbRepository,
)
from clear_vision.adapters.repositories.in_memory import VideoInMemoryRepository, InferenceInMemoryRepository


class Container(containers.DeclarativeContainer):
    wiring_config = containers.WiringConfiguration(
        modules=["clear_vision.adapters"],
        packages=["clear_vision.entrypoints.api.routers"],
    )

    frame_sampler = providers.Factory(Cv2FullVideoFrameSampler)

    detector = providers.Factory(
        LlavaTargetDetector,
        frame_sampler
    )

    video_repository = providers.Factory(
        VideoDynamoDbRepository
        # VideoInMemoryRepository
    )

    inference_repository = providers.Factory(
        InferenceDynamoDbRepository
        # InferenceInMemoryRepository
    )
