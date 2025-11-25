import typing as t
import uuid

from pydantic import BaseModel, ConfigDict, Field

from clear_vision.domain.value_objects import ImageTargetDetection, VideoFrame


class Entity(BaseModel):
    """
    Base class for entities in Domain Driven Design.
    Entities have a unique identity and are defined
    by their attributes' values.
    """

    uid: t.Union[uuid.UUID, str] = Field(default_factory=uuid.uuid4)

    def __eq__(self, other):
        if isinstance(other, type(self)):
            return self.uid == other.uid
        return False

    def __hash__(self):
        return hash(self.uid)

    def __repr__(self):
        return f"{type(self).__name__}(uid={self.uid!r})"


class Video(Entity):
    video_path: str
    frames: t.List[VideoFrame]


# TODO TRANSFORM THIS INTO AN AGGREGATE
class InferenceResult(Entity):
    video_uid: str
    target: str
    detections: t.List[ImageTargetDetection]
