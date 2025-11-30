import typing as t

from abc import ABC, abstractmethod


from clear_vision.domain.entities import Inference, Video


class VideoRepositoryInterface(ABC):

    @abstractmethod
    def add(self, video: Video) -> Video: ...

    @abstractmethod
    def get(self, uid: str) -> t.Optional[Video]: ...

    @abstractmethod
    def get_all(self) -> t.Optional[t.List[Video]]: ...


class InferenceRepositoryInterface(ABC):

    @abstractmethod
    def add(self, inference: Inference) -> Inference: ...

    @abstractmethod
    def get(self, uid: str) -> t.Optional[Inference]: ...

    @abstractmethod
    def get_all(self) -> t.Optional[t.List[Inference]]: ...

    @abstractmethod
    def get_where(self, where_dict: t.Dict[str, t.Union[str, int]]) -> t.Optional[t.List[Inference]]:
        ...