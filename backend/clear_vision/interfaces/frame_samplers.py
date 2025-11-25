import typing as t

from clear_vision.domain.value_objects import VideoFrame


class FrameSamplerInterface:

    def sample(self, video_path: str) -> t.List[VideoFrame]:
        raise NotImplementedError()
