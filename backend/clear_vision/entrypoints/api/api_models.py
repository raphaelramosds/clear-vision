import typing as t
from pydantic import BaseModel
from clear_vision.domain.dtos import VideoWithThumbnailDTO
from clear_vision.domain.entities import Inference, Video


class BaseResponse(BaseModel):
    message: str


class GetVideoResponse(BaseResponse):
    content: VideoWithThumbnailDTO


class GetVideosResponse(BaseResponse):
    content: t.List[VideoWithThumbnailDTO]


class AddInferenceResponse(BaseResponse):
    content: Inference
