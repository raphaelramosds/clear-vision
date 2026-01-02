import typing as t

from clear_vision.domain.value_objects import VideoFrame


class FrameSamplerInterface:

    def sample(self, video_path: str) -> t.List[VideoFrame]:
        """
        Extract all frames from the video.
        """
        raise NotImplementedError()
    
    def sample_thumbnail(self, video_path:str) -> bytes:
        """
        Extract the first frame of the video to use as a thumbnail.
        """
        raise NotImplementedError
