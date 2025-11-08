import torch
from transformers import LlavaProcessor, LlavaForConditionalGeneration

from typing import Any, List
from clear_vision.interfaces.models import VideoQAModelInterface


class LlavaVideoQAModel(VideoQAModelInterface):

    def generate(self, user_prompt: str, num_frames: int, frames: List[Any]) -> str:
        model_id = "llava-hf/llava-interleave-qwen-0.5b-hf"
        processor = LlavaProcessor.from_pretrained(model_id)
        model = LlavaForConditionalGeneration.from_pretrained(
            model_id, device_map="auto", dtype=torch.float16
        )
        toks = "<image>" * num_frames
        prompt = (
            "<|im_start|>user"
            + toks
            + f"\n{user_prompt}<|im_end|><|im_start|>assistant"
        )
        inputs = processor(text=prompt, images=frames, return_tensors="pt").to(
            model.device, model.dtype
        )
        output = model.generate(**inputs, max_new_tokens=100, do_sample=False)

        return processor.decode(output[0][2:], skip_special_tokens=True)[
                len(user_prompt) + 10 :
            ]
