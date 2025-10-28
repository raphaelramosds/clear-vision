import sys
import os

# Add clear_vision module to PYTHONPATH
MODULE_DIR = os.path.dirname(
    os.path.dirname(os.path.dirname(os.path.dirname(__file__)))
)
sys.path.append(MODULE_DIR)

from clear_vision.adapters.strategies import ResolveUrlStrategy
from clear_vision.domain.command import SendMessageCommand
from clear_vision.use_cases.send_message import SendMessageUseCase
from clear_vision.domain.value_objects import VideoUrl


def main():
    send_message_use_case = SendMessageUseCase()
    command = SendMessageCommand(
        path_to_video=ResolveUrlStrategy().execute(
            video=VideoUrl(
                url="https://huggingface.co/spaces/merve/llava-interleave/resolve/main/cats_1.mp4"
            )
        ),
        user_prompt="What is the cat doing on this video?",
    )
    send_message_use_case(command=command)


if __name__ == "__main__":
    main()
