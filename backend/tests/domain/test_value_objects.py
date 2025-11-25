import pytest
from clear_vision.domain.value_objects import ImageTargetDetection
from tests.fakes import FakeValueObject


def test_value_object_is_immutable():
    fake = FakeValueObject(a=1, b="x")

    with pytest.raises(Exception):
        fake.a = 2


def test_value_object_equality_same_values():
    fake1 = FakeValueObject(a=1, b="x")
    fake2 = FakeValueObject(a=1, b="x")

    assert fake1 == fake2
    assert hash(fake1) == hash(fake2)


def test_value_object_equality_different_values():
    fake1 = FakeValueObject(a=1, b="x")
    fake2 = FakeValueObject(a=2, b="x")

    assert fake1 != fake2


def test_value_object_hash_consistent():
    fake = FakeValueObject(a=1, b="x")

    h1 = hash(fake)
    h2 = hash(fake)

    assert h1 == h2


def test_value_object_repr():
    fake = FakeValueObject(a=1, b="x")

    repr_str = repr(fake)

    assert "FakeValueObject" in repr_str
    assert "a" in repr_str
    assert "1" in repr_str
    assert "b" in repr_str


def test_image_target_detection_creation():
    image_target_detection = ImageTargetDetection(ts=1.0, exists=True)

    assert image_target_detection.ts == 1.0
    assert image_target_detection.exists is True
    assert image_target_detection.description is None


def test_image_target_detection_optional_description():
    image_target_detection = ImageTargetDetection(
        ts=0.5,
        exists=False,
        description="closed",
    )

    assert image_target_detection.description == "closed"
