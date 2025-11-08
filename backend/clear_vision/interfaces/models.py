from abc import ABC, abstractmethod
from typing import Any, List


class VideoQAModelInterface(ABC):

    @abstractmethod
    def generate(self, user_prompt: str, num_frames: int, frames: List[Any]) -> str: ...
