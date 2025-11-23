from abc import ABC
from typing import Any


class FrameSamplerInterface(ABC):

    def sample(self, video_path: str) -> Any:
        raise NotImplementedError()
