import numpy as np
import typing as t

from abc import ABC
from clear_vision.domain.value_objects import TargetDetection, VideoFrame


class GeneralTargetDetectorInterface(ABC):

    def process_frame(self, frame: np.ndarray, target: str) -> str:
        raise NotImplementedError()

    def parse_model_response(self, model_response: str) -> dict:
        raise NotImplementedError()

    def detect(self, video_frame: VideoFrame, target: str) -> TargetDetection:
        processed = self.process_frame(frame=video_frame.data, target=target)
        parsed = self.parse_model_response(model_response=processed)

        return TargetDetection(
            ts=video_frame.ts,
            target_exists=parsed["target_exists"],
            description=parsed["description"],
        )

    def detect_many(self, video_frames: t.List[VideoFrame], target: str):
        return [
            self.detect(video_frame=video_frame, target=target)
            for video_frame in video_frames
        ]
