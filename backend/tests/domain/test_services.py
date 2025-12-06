import pytest
from clear_vision.adapters.repositories.in_memory import (
    InferenceInMemoryRepository,
    VideoInMemoryRepository,
)
from clear_vision.domain.entities import Video
from clear_vision.domain.services import InferenceService, VideoService
from tests.fakes import FakeDetector, FakeFrameSampler


def test_add_video():
    video_repository = VideoInMemoryRepository(videos=[])
    video_service = VideoService(video_repository=video_repository)

    video = video_service.add_video("/tmp/test.mp4")
    videos = video_repository.get_all()

    assert video.video_path == "/tmp/test.mp4"
    assert len(videos) > 0


def test_get_video_by_uid():
    video_repository = VideoInMemoryRepository(videos=[])
    video_service = VideoService(video_repository=video_repository)

    new_video = video_service.add_video("/tmp/test.mp4")
    video = video_service.get_video(str(new_video.uid))

    assert str(video.uid) == str(new_video.uid)
    assert video.video_path == new_video.video_path


def test_get_video_not_found():
    video_repository = VideoInMemoryRepository()
    video_service = VideoService(video_repository=video_repository)

    with pytest.raises(Exception):
        video_service.get_video("invalid-uid")


def test_add_inference():
    inference_repository = InferenceInMemoryRepository(inferences=[])

    video_repository = VideoInMemoryRepository(
        videos=[Video(video_path="/tmp/test.mp4")]
    )

    inference_service = InferenceService(
        inference_repository=inference_repository,
        video_service=VideoService(video_repository=video_repository),
    )

    videos = video_repository.get_all()
    video = videos[0]

    inference = inference_service.add_inference(
        video_uid=str(video.uid),
        target="a fake target",
        detector=FakeDetector(),
        frame_sampler=FakeFrameSampler(),
    )

    assert inference.video_uid == str(video.uid)
    assert inference.target == "a fake target"
    assert len(inference_repository.get_all()) == 1


def test_get_inference_by_uid():
    inference_repository = InferenceInMemoryRepository(inferences=[])
    video_repository = VideoInMemoryRepository(
        videos=[Video(video_path="/tmp/test.mp4")]
    )

    inference_service = InferenceService(
        inference_repository=inference_repository,
        video_service=VideoService(video_repository=video_repository),
    )

    video = video_repository.get_all()[0]

    new_inf = inference_service.add_inference(
        video_uid=str(video.uid),
        target="car",
        detector=FakeDetector(),
        frame_sampler=FakeFrameSampler(),
    )

    inf = inference_service.get_inference(str(new_inf.uid))

    assert str(inf.uid) == str(new_inf.uid)
    assert inf.target == new_inf.target
    assert inf.video_uid == new_inf.video_uid


def test_get_video_inferences():
    inference_repository = InferenceInMemoryRepository(inferences=[])
    video_repository = VideoInMemoryRepository(
        videos=[Video(video_path="/tmp/test.mp4")]
    )

    detector = FakeDetector()
    frame_sampler = FakeFrameSampler()

    inference_service = InferenceService(
        inference_repository=inference_repository,
        video_service=VideoService(video_repository=video_repository),
    )

    video = video_repository.get_all()[0]

    inf1 = inference_service.add_inference(
        str(video.uid), target="person", detector=detector, frame_sampler=frame_sampler
    )
    inf2 = inference_service.add_inference(
        str(video.uid), target="car", detector=detector, frame_sampler=frame_sampler
    )

    inferences = inference_service.get_video_inferences(video_uid=str(video.uid))

    assert len(inferences) == 2
    assert {i.uid for i in inferences} == {inf1.uid, inf2.uid}


def test_get_inference_not_found():
    inference_repository = InferenceInMemoryRepository(inferences=[])
    video_repository = VideoInMemoryRepository(
        videos=[Video(video_path="/tmp/test.mp4")]
    )

    inference_service = InferenceService(
        inference_repository=inference_repository,
        video_service=VideoService(video_repository=video_repository),
    )

    with pytest.raises(Exception):
        inference_service.get_inference("invalid-uid")
