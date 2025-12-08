import typing as t
from pydantic import BaseModel
from clear_vision.domain.dtos import VideoWithThumbnailDTO, InferenceSimplifiedDTO
from clear_vision.domain.entities import Video


class BaseResponse(BaseModel):
    message: str

class UploadVideoResponse(BaseResponse):
    content: Video

class GetVideoResponse(BaseResponse):
    content: VideoWithThumbnailDTO


class GetVideosResponse(BaseResponse):
    content: t.List[VideoWithThumbnailDTO]


class AddInferenceResponse(BaseResponse):
    uid: str
    video_uid: str


class GetVideoInferencesResponse(BaseResponse):
    content: t.List[InferenceSimplifiedDTO]
