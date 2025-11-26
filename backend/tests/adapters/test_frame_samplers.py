import numpy
import pytest
from clear_vision.adapters.frame_samplers import Cv2FullVideoFrameSampler
from clear_vision.domain.value_objects import VideoFrame


def test_timestamps_are_correct(mock_video_capture):
    mock_video_capture.configure(num_frames=15, fps=5)

    sampler = Cv2FullVideoFrameSampler()
    samples = sampler.sample("fake/path.mp4")

    assert len(samples) == 3
    assert samples[0].ts == 0.0
    assert samples[1].ts == 1.0
    assert samples[2].ts == 2.0

def test_raises_if_video_cannot_open(mock_video_capture):
    mock_video_capture.configure(num_frames=0, fps=5, opened=False)

    sampler = Cv2FullVideoFrameSampler()

    with pytest.raises(ValueError, match="Could not open video file"):
        sampler.sample("fake/path.mp4")

def test_raises_if_fps_is_zero(mock_video_capture):
    mock_video_capture.configure(num_frames=10, fps=0)

    sampler = Cv2FullVideoFrameSampler()

    with pytest.raises(ValueError, match="FPS could not be read"):
        sampler.sample("fake/path.mp4")

def test_raises_if_no_frames_extracted(mock_video_capture):
    mock_video_capture.configure(num_frames=0, fps=5)

    sampler = Cv2FullVideoFrameSampler()

    with pytest.raises(ValueError, match="No frames extracted"):
        sampler.sample("fake/path.mp4")

def test_frame_conversion_to_rgb(mock_video_capture):
    frame_bgr = numpy.array([[[255, 0, 0]]], dtype=numpy.uint8)  # blue BGR

    mock_video_capture.isOpened.return_value = True
    mock_video_capture.get.return_value = 1 
    mock_video_capture.read.side_effect = [
        (True, frame_bgr),
        (False, None),
    ]

    sampler = Cv2FullVideoFrameSampler()
    samples = sampler.sample("fake/path.mp4")

    assert isinstance(samples[0], VideoFrame)

    assert samples[0].data[0][0].tolist() == [0, 0, 255] # blue RGB