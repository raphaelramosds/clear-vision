import cv2
import matplotlib.pyplot as plt

from clear_vision.adapters.frame_samplers import Cv2FullVideoFrameSampler


def show_frame(frame: cv2.typing.MatLike) -> None:
    plt.imshow(frame)
    plt.axis("off")
    plt.show()


def convert_frame_to_bytes(frame: cv2.typing.MatLike) -> bytes:
    ok, buffer = cv2.imencode(".jpg", frame)
    image_bytes = buffer.tobytes()

    return image_bytes

def sample_video(video_path):
    samples = Cv2FullVideoFrameSampler().sample(video_path)
    ts_frames = [(sample.ts, sample.data) for sample in samples]
    ts = [tf[0] for tf in ts_frames]
    frames = [tf[1] for tf in ts_frames]
    return ts, frames
