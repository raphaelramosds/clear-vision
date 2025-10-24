import sys
import os

# Add clear_vision module to PYTHONPATH
MODULE_DIR = os.path.dirname(
    os.path.dirname(os.path.dirname(os.path.dirname(__file__)))
)
sys.path.append(MODULE_DIR)

from clear_vision.services.send_message import call

call(
    url="https://huggingface.co/spaces/merve/llava-interleave/resolve/main/cats_1.mp4",
    user_prompt="Is there a cat on this videdo?",
)