import cv2
import torch

from transformers import LlavaProcessor, LlavaForConditionalGeneration
from PIL import Image

from clear_vision.domain.command import SendMessageCommand


class SendMessageUseCase:

    def __call__(
        self,
        command: SendMessageCommand,
    ) -> None:

        # Sample frames
        num_frames = 6
        path = command.path_to_video
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

        # Process video with transformer accordingly to user prompt
        model_id = "llava-hf/llava-interleave-qwen-0.5b-hf"
        processor = LlavaProcessor.from_pretrained(model_id)
        model = LlavaForConditionalGeneration.from_pretrained(
            model_id, device_map="auto", dtype=torch.float16
        )
        toks = "<image>" * num_frames
        prompt = (
            "<|im_start|>user"
            + toks
            + f"\n{command.user_prompt}<|im_end|><|im_start|>assistant"
        )
        inputs = processor(text=prompt, images=frames, return_tensors="pt").to(
            model.device, model.dtype
        )
        output = model.generate(**inputs, max_new_tokens=100, do_sample=False)

        print(
            processor.decode(output[0][2:], skip_special_tokens=True)[
                len(command.user_prompt) + 10 :
            ]
        )
