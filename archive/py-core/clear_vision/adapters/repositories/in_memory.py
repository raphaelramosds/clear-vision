import typing as t

from clear_vision.domain.entities import Inference, Video
from clear_vision.interfaces.repositories import (
    InferenceRepositoryInterface,
    VideoRepositoryInterface,
)


class VideoInMemoryRepository(VideoRepositoryInterface):

    def __init__(self, videos: t.List[Video] = []):
        self._videos = videos

    def add(self, video):
        self._videos.append(video)
        return video

    def get(self, uid):
        return next(
            (video for video in self._videos if str(video.uid) == str(uid)), None
        )

    def get_all(self):
        return self._videos


class InferenceInMemoryRepository(InferenceRepositoryInterface):

    def __init__(self, inferences: t.List[Inference] = []):
        self._inferences = inferences

    def add(self, inference):
        self._inferences.append(inference)
        return inference

    def get(self, uid):
        return next(
            (
                inference
                for inference in self._inferences
                if str(inference.uid) == str(uid)
            ),
            None,
        )

    def get_all(self):
        return self._inferences

    def get_where(self, where_dict):
        results = []

        for item in self._inferences:
            match = True
            for field, value in where_dict.items():
                if not hasattr(item, field) or getattr(item, field) != value:
                    match = False
                    break
            if match:
                results.append(item)

        return results