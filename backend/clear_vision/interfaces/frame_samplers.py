from abc import ABC, abstractmethod
from typing import Any, List


class FrameSamplerInterface(ABC):

    @abstractmethod
    def sample(self, path: str, num_frames: int) -> List[Any]: ...
