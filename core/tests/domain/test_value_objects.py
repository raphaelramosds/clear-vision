from decimal import Decimal
from clear_vision.domain.value_objects import TargetDetection

def test_image_target_detection_creation():
    image_target_detection = TargetDetection(ts=Decimal(1.0), target_exists=True)

    assert image_target_detection.ts == Decimal(1.0)
    assert image_target_detection.target_exists is True
    assert image_target_detection.description is None


def test_image_target_detection_optional_description():
    image_target_detection = TargetDetection(
        ts=Decimal(1.0),
        target_exists=False,
        description="closed",
    )

    assert image_target_detection.description == "closed"
