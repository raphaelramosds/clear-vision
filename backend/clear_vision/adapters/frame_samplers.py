from typing import Any, List
import cv2

from PIL import Image

from clear_vision.interfaces.frame_samplers import FrameSamplerInterface


class Cv2VideoCaptureFrameSampler(FrameSamplerInterface):

    def sample(self, path: str, num_frames: int) -> List[Any]:
        video = cv2.VideoCapture(path)
        total_frames = int(video.get(cv2.CAP_PROP_FRAME_COUNT))
        interval = total_frames // num_frames
        frames = []

        for i in range(total_frames):
            ret, frame = video.read()
            pil_img = Image.fromarray(cv2.cvtColor(frame, cv2.COLOR_BGR2RGB))
            if not ret:
                continue
            if i % interval == 0:
                frames.append(pil_img)

        video.release()
        frames = frames[:num_frames]

        return frames
