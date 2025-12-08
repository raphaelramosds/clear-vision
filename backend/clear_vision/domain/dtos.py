import base64
import typing as t

from decimal import Decimal
from pydantic import BaseModel, field_serializer
from clear_vision.domain.entities import Inference


class VideoWithThumbnailDTO(BaseModel):
    uid: str
    thumbnail: str


class InferenceSimplifiedDTO(BaseModel):
    uid: str
    timestamps: t.List[Decimal]

    @classmethod
    def from_inference(cls, inference: Inference):
        return cls(
            uid=inference.uid,
            timestamps=[detection.ts for detection in inference.detections if detection.target_exists]
        )
