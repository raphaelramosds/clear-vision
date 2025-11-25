from clear_vision.domain.value_objects import ValueObject


class FakeValueObject(ValueObject):
    a: int
    b: str | None = None
