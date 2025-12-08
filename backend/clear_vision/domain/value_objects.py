import typing as t
import numpy as np
from pydantic import BaseModel, ConfigDict, field_serializer
from decimal import Decimal


class ValueObject(BaseModel):
    """
    Base class for value objects.
    Value objects are immutable and defined by their attributes' values.
    """

    model_config = ConfigDict(frozen=True, arbitrary_types_allowed=True)

    def __repr__(self):
        return f"{type(self).__name__}({self.model_dump()!r})"


class TargetDetection(ValueObject):
    ts: Decimal
    target_exists: bool


class VideoFrame(ValueObject):
    ts: Decimal
    data: np.ndarray

    model_config = ConfigDict(arbitrary_types_allowed=True)
