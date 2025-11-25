from clear_vision.domain.value_objects import ValueObject
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
