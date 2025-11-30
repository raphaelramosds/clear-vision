import numpy as np

from clear_vision.domain.value_objects import TargetDetection, ValueObject, VideoFrame
from clear_vision.interfaces.detectors import GeneralTargetDetectorInterface
from clear_vision.interfaces.frame_samplers import FrameSamplerInterface
from clear_vision.interfaces.models import (
    ChatbotModelInterface,
    HFChatbotModelInterface,
)


class FakeValueObject(ValueObject):
    a: int
    b: str | None = None


class FakeChatbotModel(ChatbotModelInterface):

    def _load_model(self, model_id):
        return f"fake-chatbot-{model_id}"


class FakeHFChatbotModel(HFChatbotModelInterface):

    def _load_model(self, model_id):
        return f"fake-hf-chatbot-{model_id}"


class FakeDetector(GeneralTargetDetectorInterface):

    def process_frame(self, frame, target):
        return ""

    def parse_model_response(self, model_response):
        return {}

    def run_video_target_detection(self, video_path, target, frame_sampler):
        return [
            TargetDetection(
                description="Fake target found", target_exists=True, ts=1.0
            ),
            TargetDetection(
                description="Fake target found", target_exists=True, ts=4.0
            ),
        ]


class FakeFrameSampler(FrameSamplerInterface):

    def sample(self, video_path):
        return [VideoFrame(data=np.array([0]), ts=1.0)]
