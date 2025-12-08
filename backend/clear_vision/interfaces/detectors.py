import typing as t

from abc import ABC, abstractmethod
from clear_vision.domain.value_objects import TargetDetection, VideoFrame


class GeneralTargetDetectorInterface(ABC):

    @abstractmethod
    def __call__(self, video_path: str, target: str) -> t.List[TargetDetection]:
        raise NotImplementedError()
