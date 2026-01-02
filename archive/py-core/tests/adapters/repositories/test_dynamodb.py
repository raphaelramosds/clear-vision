import pytest

from decimal import Decimal
from clear_vision.domain.entities import Video, Inference
from clear_vision.adapters.repositories.dynamodb import VideoDynamoDbRepository, InferenceDynamoDbRepository
from clear_vision.domain.value_objects import TargetDetection
from clear_vision.infra.dynamodb.helpers import ddb_start_tables, ddb_refresh_tables


# NOTE. BEFORE RUNNING THESE TESTS MAKE SURE A DYNAMO DB INSTANCE IS RUNNING LOCALLY
# 1. DEPLOY dynamodb SERVICE: docker compose up dynamodb-pytest -d
# 2. SET THIS ENVIRONMENT VARIABLE ON YOUR HOST: export DYNAMODB_ENDPOINT_URL="http://0.0.0.0:8005"

@pytest.fixture(scope="session")
def before_all():
    ddb_refresh_tables()


def test_add_video():
    video_repository = VideoDynamoDbRepository()
    v = Video(video_path="/tmp/video.mp4")

    saved = video_repository.add(v)

    assert saved.uid == v.uid
    assert saved.video_path == "/tmp/video.mp4"


def test_get_video():
    video_repository = VideoDynamoDbRepository()
    v = Video(video_path="/tmp/video.mp4")

    video_repository.add(v)
    stored = video_repository.get(v.uid)

    assert stored is not None
    assert stored.uid == v.uid
    assert stored.video_path == "/tmp/video.mp4"


def test_add_inference():
    inference_repository = InferenceDynamoDbRepository()
    inference = Inference(
        video_uid="video-123",
        target="a person",
        detections=[
            TargetDetection(target_exists=True, ts=Decimal(4.0)),
            TargetDetection(target_exists=True, ts=Decimal(6.0)),
        ]
    )
    saved = inference_repository.add(inference)
    fetched = inference_repository.get(inference.uid)

    assert saved.uid == inference.uid
    assert fetched is not None
    assert fetched.uid == inference.uid
    assert fetched.video_uid == "video-123"
    assert fetched.target == "a person"