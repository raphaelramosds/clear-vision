import cv2
import matplotlib.pyplot as plt


def extract_frames(video_path: str):
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


def show_frame(frame: cv2.typing.MatLike) -> None:
    plt.imshow(frame)
    plt.axis("off")
    plt.show()

def convert_frame_to_bytes(frame: cv2.typing.MatLike) -> bytes:
    ok, buffer = cv2.imencode(".jpg", frame)
    image_bytes = buffer.tobytes()
    return image_bytes