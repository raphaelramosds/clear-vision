import cv2

from clear_vision.infra.interfaces.frame_samplers import FrameSamplerInterface


class Cv2FullVideoFrameSampler(FrameSamplerInterface):

    def sample(self, video_path):
        cap = cv2.VideoCapture(video_path)

        if not cap.isOpened():
            raise ValueError(f"Could not open video file: {video_path}")

        fps = cap.get(cv2.CAP_PROP_FPS)
        if fps == 0:
            raise ValueError("FPS could not be read. Video file is invalid or corrupted.")

        interval = int(fps)
        frames, timestamps = [], []
        i = 0

        while True:
            ret, frame = cap.read()
            if not ret:
                break
            if i % interval == 0:
                rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
                frames.append(rgb)
                timestamps.append(i / fps)
            i += 1

        cap.release()

        if len(frames) == 0:
            raise ValueError(
                "No frames extracted from video. Check the file path or permissions."
            )

        return frames, timestamps
