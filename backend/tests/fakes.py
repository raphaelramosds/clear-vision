import numpy as np

from decimal import Decimal
from clear_vision.domain.value_objects import TargetDetection, VideoFrame
from clear_vision.interfaces.detectors import GeneralTargetDetectorInterface
from clear_vision.interfaces.frame_samplers import FrameSamplerInterface
from clear_vision.interfaces.models import (
    ChatbotModelInterface,
    HFChatbotModelInterface,
)


class FakeChatbotModel(ChatbotModelInterface):

    def _load_model(self):
        return f"fake-chatbot"


class FakeHFChatbotModel(HFChatbotModelInterface):

    def _load_model(self):
        return f"fake-hf-chatbot"


class FakeDetector(GeneralTargetDetectorInterface):

    def __call__(self, video_path, target):
        return [
            TargetDetection(
                description="Fake target found", target_exists=True, ts=Decimal(1.0)
            ),
            TargetDetection(
                description="Fake target found", target_exists=True, ts=Decimal(4.0)
            ),
        ]


class FakeFrameSampler(FrameSamplerInterface):

    def sample(self, video_path):
        return [VideoFrame(data=np.array([0]), ts=Decimal(1.0))]

    def sample_thumbnail(self, video_path: str) -> bytes:
        return bytearray()
