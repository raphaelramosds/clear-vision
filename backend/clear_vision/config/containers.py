from dependency_injector import containers, providers

from clear_vision.adapters.detectors import LlavaTargetDetector
from clear_vision.adapters.frame_samplers import Cv2FullVideoFrameSampler
from clear_vision.adapters.repositories import (
    InferenceInMemoryRepository,
    VideoInMemoryRepository,
)


class Container(containers.DeclarativeContainer):

    wiring_config = containers.WiringConfiguration(
        modules=["clear_vision.adapters"],
        packages=["clear_vision.entrypoints.api.routers"],
    )

    frame_sampler = providers.Factory(Cv2FullVideoFrameSampler)

    detector = providers.Factory(LlavaTargetDetector)

    video_repository = providers.Factory(VideoInMemoryRepository)

    inference_repository = providers.Factory(InferenceInMemoryRepository)
