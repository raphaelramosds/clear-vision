import cv2
import matplotlib.pyplot as plt


def show_frame(frame: cv2.typing.MatLike) -> None:
    plt.imshow(frame)
    plt.axis("off")
    plt.show()


def convert_frame_to_bytes(frame: cv2.typing.MatLike) -> bytes:
    ok, buffer = cv2.imencode(".jpg", frame)
    image_bytes = buffer.tobytes()

    return image_bytes
