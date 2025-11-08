from dependency_injector import containers, providers

from clear_vision.adapters.frame_samplers import Cv2VideoCaptureFrameSampler
from clear_vision.adapters.models import LlavaVideoQAModel


class Container(containers.DeclarativeContainer):

    wiring_config = containers.WiringConfiguration(
        
        # wiring on .py files
        modules=[],

        # recursive wiring on directories with __init__.py
        packages=[
            "clear_vision.adapters",
            "clear_vision.entrypoints.api.routers",
        ],
    )

    frame_sampler = providers.Factory(Cv2VideoCaptureFrameSampler)

    model = providers.Factory(LlavaVideoQAModel)
