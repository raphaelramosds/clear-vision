from abc import ABC, abstractmethod

from clear_vision.domain.entities import Video


class ResolvePathStrategyInterface(ABC):

    @abstractmethod
    def execute(self, video: Video)->str:
        raise NotImplementedError
