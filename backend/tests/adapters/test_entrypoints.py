from unittest.mock import patch

import pytest

from clear_vision.adapters.repositories.in_memory import (
    InferenceInMemoryRepository,
    VideoInMemoryRepository,
)
from clear_vision.domain.entities import Inference, Video
from clear_vision.entrypoints.api.main import app
from tests.fakes import FakeDetector, FakeFrameSampler


@pytest.mark.asyncio
async def test_upload_video(tmp_path, async_client):
    video_repository = VideoInMemoryRepository(videos=[])
    test_file = tmp_path / "fake.mp4"
    test_file.write_bytes(b"fake video bytes")

    with app.container.video_repository.override(video_repository):
        with open(test_file, "rb") as f:
            response = await async_client.post(
                "/videos/upload/",
                files={"video": ("fake.mp4", f, "video/mp4")},
            )
            assert response.status_code == 200

    json = response.json()
    assert json["message"] == "File uploaded"
    assert "file" in json
    assert "video_path" in json["file"]
    assert json["file"]["video_path"].endswith(".mp4")


@pytest.mark.asyncio
async def test_get_video_success(async_client):
    video_repository = VideoInMemoryRepository(
        videos=[
            Video(video_path="fake-video-1"),
            Video(video_path="fake-video-2"),
        ]
    )

    video = video_repository.get_all()[0]

    with app.container.video_repository.override(video_repository):
        frame_sampler = app.container.frame_sampler()

        with patch.object(
                frame_sampler,
                "sample_thumbnail",
                return_value=b"fake-thumbnail",
        ) as mock_sample_thumbnail:
            # Inject our mocked frame sampler
            with app.container.frame_sampler.override(frame_sampler):
                response = await async_client.get(f"/videos/{video.uid}")
                mock_sample_thumbnail.assert_called_once_with(video.video_path)

    assert response.status_code == 200

    body = response.json()
    assert body["content"]["uid"] == str(video.uid)


@pytest.mark.asyncio
async def test_add_inference(async_client):
    video_repository = VideoInMemoryRepository(
        videos=[Video(video_path="fake-video.mp4")]
    )
    inference_repository = InferenceInMemoryRepository(inferences=[])

    all_videos = video_repository.get_all()
    video = all_videos[0]

    with (
        app.container.video_repository.override(video_repository),
        app.container.inference_repository.override(inference_repository),
        app.container.frame_sampler.override(FakeFrameSampler()),
        app.container.detector.override(FakeDetector()),
    ):
        response = await async_client.post(
            "/inferences/",
            params={
                "video_uid": str(video.uid),
                "target": "fake target",
            },
        )

    assert response.status_code == 200

    body = response.json()
    assert body["content"]["video_uid"] == str(video.uid)
    assert body["content"]["target"] == "fake target"
    assert "detections" in body["content"]
    assert len(body["content"]["detections"]) == 2


@pytest.mark.asyncio
async def test_get_inference(async_client):
    detector = FakeDetector()

    video_repository = VideoInMemoryRepository(
        videos=[Video(video_path="fake-video.mp4")]
    )

    all_videos = video_repository.get_all()
    video = all_videos[0]

    inference_repository = InferenceInMemoryRepository(
        inferences=[
            Inference(
                detections=detector(
                    video_path=video.video_path,
                    target="fake target",
                ),
                video_uid=str(video.uid),
                target="fake target",
            )
        ]
    )

    inferences = inference_repository.get_all()
    inference = inferences[0]

    with (
        app.container.inference_repository.override(inference_repository),
        app.container.video_repository.override(video_repository),
    ):
        response = await async_client.get(f"/inferences/{inference.uid}")

    assert response.status_code == 200

    body = response.json()
    assert body["uid"] == str(inference.uid)
    assert body["video_uid"] == str(video.uid)
    assert body["target"] == "fake target"


@pytest.mark.asyncio
async def test_get_video_inferences(async_client):
    detector = FakeDetector()

    video_repository = VideoInMemoryRepository(
        videos=[Video(video_path="fake-video.mp4")]
    )

    video = video_repository.get_all()[0]

    inference_repository = InferenceInMemoryRepository(
        inferences=[
            Inference(
                detections=detector(
                    video_path=video.video_path,
                    target="target-1",
                ),
                video_uid=str(video.uid),
                target="target-1",
            ),
            Inference(
                detections=detector(
                    video_path=video.video_path,
                    target="target-2",
                ),
                video_uid=str(video.uid),
                target="target-2",
            ),
        ]
    )

    with (
        app.container.inference_repository.override(inference_repository),
        app.container.video_repository.override(video_repository),
    ):
        response = await async_client.get(
            f"/videos/{video.uid}/inferences"
        )

    assert response.status_code == 200

    body = response.json()

    assert isinstance(body, list)
    assert len(body) == 2

    for inf in body:
        assert inf["video_uid"] == str(video.uid)

    targets = {inf["target"] for inf in body}
    assert targets == {"target-1", "target-2"}
