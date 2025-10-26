import uuid
import requests
import cv2
import torch

from transformers import LlavaProcessor, LlavaForConditionalGeneration
from PIL import Image


def call_send_message(url: str, user_prompt: str) -> None:

    def sample_frames(url: str, num_frames: int) -> list:
        response = requests.get(url)
        path_id = str(uuid.uuid4())

        path = f"./backend/public/{path_id}.mp4"

        with open(path, "wb") as f:
            f.write(response.content)

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
        return frames[:num_frames]

    model_id = "llava-hf/llava-interleave-qwen-0.5b-hf"

    processor = LlavaProcessor.from_pretrained(model_id)

    model = LlavaForConditionalGeneration.from_pretrained(
        model_id, device_map="auto", dtype=torch.float16
    )

    video = sample_frames(url, 6)
    toks = "<image>" * 6
    prompt = (
        "<|im_start|>user" + toks + f"\n{user_prompt}<|im_end|><|im_start|>assistant"
    )
    inputs = processor(text=prompt, images=video, return_tensors="pt").to(
        model.device, model.dtype
    )
    output = model.generate(**inputs, max_new_tokens=100, do_sample=False)

    print(
        processor.decode(output[0][2:], skip_special_tokens=True)[
            len(user_prompt) + 10 :
        ]
    )
