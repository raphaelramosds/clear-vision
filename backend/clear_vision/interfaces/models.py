import typing as t

from abc import ABC, abstractmethod
from transformers.utils import logging


class ChatbotModelInterface(ABC):

    def __init__(self):
        self.model = self._load_model()

    @abstractmethod
    def _load_model(self) -> t.Any: ...


class HFChatbotModelInterface(ChatbotModelInterface):

    def __init__(self):
        super().__init__()
        logging.set_verbosity_error()

    @abstractmethod
    def _load_model(self) -> t.Any: ...
