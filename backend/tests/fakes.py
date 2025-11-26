from clear_vision.domain.value_objects import TargetDetection, ValueObject, VideoFrame
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

    def generate_target_detection(
        self, target: str, sample: VideoFrame
    ) -> TargetDetection:
        return TargetDetection(
            ts=0.0,
            exists=False,
        )


class FakeHFChatbotModel(HFChatbotModelInterface):
    def _load_model(self, model_id):
        return f"fake-hf-chatbot-{model_id}"

    def generate_target_detection(
        self, target: str, sample: VideoFrame
    ) -> TargetDetection:
        return TargetDetection(
            ts=3.0,
            exists=True,
        )