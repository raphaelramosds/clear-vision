import typing as t
from pydantic import BaseModel
from clear_vision.domain.entities import Inference, Video


class BaseResponse(BaseModel):
    message: str

class GetVideosResponse(BaseResponse):
    content: t.List[Video]

class AddInferenceResponse(BaseResponse):
    content: Inference