import sys
import os


# Add clear_vision module to PYTHONPATH
MODULE_DIR = os.path.dirname(
    os.path.dirname(os.path.dirname(os.path.dirname(__file__)))
)
sys.path.append(MODULE_DIR)

from dependency_injector.wiring import inject, Provide

from clear_vision.domain.value_objects import VideoUrl
from clear_vision.config.containers import Container
from clear_vision.adapters.strategies import ResolveUrlStrategy
from clear_vision.domain.command import SendMessageCommand
from clear_vision.use_cases.send_message import SendMessageUseCase
from clear_vision.interfaces.frame_samplers import FrameSamplerInterface
from clear_vision.interfaces.models import VideoQAModelInterface


@inject
def main(
    frame_sampler: FrameSamplerInterface = Provide[Container.frame_sampler],
    model: VideoQAModelInterface = Provide[Container.model],
):
    send_message_use_case = SendMessageUseCase()
    command = SendMessageCommand(
        path_to_video=ResolveUrlStrategy().execute(
            video=VideoUrl(
                url="https://huggingface.co/spaces/merve/llava-interleave/resolve/main/cats_1.mp4"
            )
        ),
        user_prompt="In which minute does a cat appear on this video?",
    )
    result = send_message_use_case(
        command=command,
        model=model,
        frame_sampler=frame_sampler,
    )
    print(result)


if __name__ == "__main__":
    container = Container()
    container.wire(modules=[__name__])

    main()
