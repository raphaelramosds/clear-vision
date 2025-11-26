import typing as t

from abc import ABC, abstractmethod
from transformers.utils import logging

from clear_vision.domain.value_objects import TargetDetection, VideoFrame


class ChatbotModelInterface(ABC):

    def __init__(self, model_id: str):
        self.model_id = model_id
        self.model = self._load_model(model_id=model_id)

    @abstractmethod
    def _load_model(self, model_id: str) -> t.Any: ...

    @abstractmethod
    def generate_target_detection(
        self, target: str, sample: VideoFrame
    ) -> TargetDetection: ...


class HFChatbotModelInterface(ChatbotModelInterface):

    def __init__(self, model_id):
        super().__init__(model_id=model_id)
        logging.set_verbosity_error()