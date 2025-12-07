import base64
from pydantic import BaseModel, field_serializer


class VideoWithThumbnailDTO(BaseModel):
    uid: str
    thumbnail: bytes

    # thumbnail should be serialized as base64
    @field_serializer("thumbnail")
    def encode_thumbnail(self, value: bytes, _info):
        return base64.b64encode(value).decode("utf-8")
