import typing as t

from clear_vision.config.logger import get_logger
from clear_vision.domain.entities import Inference, Video
from clear_vision.domain.exceptions import (
    InferenceNotFoundError,
    InferencesNotFoundError,
    VideoNotFoundError,
)
from clear_vision.interfaces.detectors import GeneralTargetDetectorInterface
from clear_vision.interfaces.frame_samplers import FrameSamplerInterface
from clear_vision.interfaces.repositories import (
    InferenceRepositoryInterface,
    VideoRepositoryInterface,
)

class VideoService:

    def __init__(self, video_repository: VideoRepositoryInterface) -> None:
        self.video_repository = video_repository

    def add_video(self, video_path: str) -> Video:
        video = self.video_repository.add(Video(video_path=video_path))
        return video

    def get_video(self, video_uid: str) -> Video:
        video = self.video_repository.get(uid=video_uid)

        if not video:
            raise VideoNotFoundError(f"Could not find video with UUID {video_uid}")

        return video
    
    def get_videos(self) -> t.Optional[t.List[Video]]:
        videos = self.video_repository.get_all()
        
        return videos


class InferenceService:

    def __init__(
        self,
        inference_repository: InferenceRepositoryInterface,
        video_service: VideoService,
    ) -> None:
        self.inference_repository = inference_repository
        self.video_service = video_service

    def add_inference(
        self,
        video_uid: str,
        target: str,
        detector: GeneralTargetDetectorInterface,
        frame_sampler: FrameSamplerInterface,
    ) -> Inference:
        video = self.video_service.get_video(video_uid=video_uid)
        detections = detector.run_video_target_detection(
            video_path=video.video_path, target=target, frame_sampler=frame_sampler
        )

        inference = Inference(target=target, video_uid=video_uid, detections=detections)
        self.inference_repository.add(inference)

        return inference

    def get_inference(self, inference_uid: str) -> Inference:
        inference = self.inference_repository.get(uid=inference_uid)

        if not inference:
            raise InferenceNotFoundError(
                f"Could not find inference with UUID {inference_uid}"
            )

        return inference

    def get_video_inferences(self, video_uid: str) -> t.Optional[t.List[Inference]]:
        inferences = self.inference_repository.get_where(
            where_dict={"video_uid": video_uid}
        )

        if not inferences:
            raise InferencesNotFoundError(
                f"Could not find inferences for video {video_uid}"
            )

        return inferences
