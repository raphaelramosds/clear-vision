import typing as t
import numpy as np
from pydantic import BaseModel, ConfigDict
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
    description: t.Optional[str] = None


class VideoFrame(ValueObject):
    data: np.ndarray
    ts: Decimal
