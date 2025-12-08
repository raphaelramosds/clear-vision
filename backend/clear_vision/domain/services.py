import base64
import typing as t

from clear_vision.config.logger import get_logger
from clear_vision.domain.dtos import VideoWithThumbnailDTO
from clear_vision.domain.entities import Inference, Video
from clear_vision.domain.exceptions import (
    EmptyRepositoryError,
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

logger = get_logger(__name__)


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
        if not videos:
            raise EmptyRepositoryError(f"There are no video records on our database")
        return videos

    def get_videos_with_thumbnail(self, frame_sampler: FrameSamplerInterface) -> t.List[VideoWithThumbnailDTO]:
        videos = self.video_repository.get_all()
        if not videos:
            raise EmptyRepositoryError(f"There are no video records on our database")
        return [
            self.get_video_with_thumbnail(video_uid=video.uid, frame_sampler=frame_sampler) for video in videos
        ]

    def get_video_with_thumbnail(self, video_uid: str, frame_sampler: FrameSamplerInterface) -> VideoWithThumbnailDTO:
        video = self.video_repository.get(uid=video_uid)
        thumbnail = ""

        if not video:
            raise VideoNotFoundError(f"Could not find video with UUID {video_uid}")
        try:
            thumbnail_bytes = frame_sampler.sample_thumbnail(video.video_path)
            thumbnail = base64.b64encode(thumbnail_bytes).decode("utf-8")
        except ValueError as e:
            logger.warning(f"Could not sample thumbnail for video %s", video.uid)

        return VideoWithThumbnailDTO(
            uid=video.uid,
            thumbnail=thumbnail,
        )


class InferenceService:

    def __init__(
            self,
            inference_repository: InferenceRepositoryInterface,
            video_repository: VideoRepositoryInterface,
    ) -> None:
        self.inference_repository = inference_repository
        self.video_repository = video_repository

    def add_inference(
            self,
            video_uid: str,
            target: str,
            detector: GeneralTargetDetectorInterface,
    ) -> Inference:
        video = self.video_repository.get(uid=video_uid)

        if not video:
            raise VideoNotFoundError(f"Video {video_uid} not found on repository")

        logger.info(f"Found video {video.video_path}. Will run target detection")

        detections = detector(
            video_path=video.video_path, target=target
        )

        logger.info(f"Detection finished. Will add it to inference repository.")

        inference = Inference(target=target, video_uid=video_uid, detections=detections)
        self.inference_repository.add(inference)

        logger.info(f"Inference {inference.uid} added.")

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
