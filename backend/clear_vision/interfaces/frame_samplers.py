import typing as t

from abc import ABC
from clear_vision.domain.value_objects import VideoFrame


class FrameSamplerInterface(ABC):

    def sample(self, video_path: str) -> t.List[VideoFrame]:
        raise NotImplementedError()
