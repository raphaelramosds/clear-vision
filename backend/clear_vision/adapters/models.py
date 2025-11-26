import json
import torch
import re
import numpy as np
import typing as t

from transformers import (
    BitsAndBytesConfig,
    LlavaForConditionalGeneration,
    LlavaProcessor,
)

from clear_vision.domain.value_objects import TargetDetection, VideoFrame
from clear_vision.interfaces.models import HFChatbotModelInterface


class LlavaInterleaveQwenModel(HFChatbotModelInterface):

    PROMPT = (
        "<|im_start|>user\n"
        "<image>\n\n"
        'Analyze the image and search for this target object: "{target}".\n'
        "Then, return ONLY JSON in the following format:\n"
        '{{"exists": boolean, "description": string}}\n'
        "Where:\n"
        '- "description": must describe ALL visible objects in the image.\n'
        '- "exists": must be true if the target object is present. Otherhise, it must be false.\n'
        "<|im_end|>\n"
        "<|im_start|>assistant\n"
    )

    def __init__(self, model_id):
        super().__init__(model_id=model_id)
        self.processor = self._load_processor()

    def _load_model(self, model_id) -> LlavaForConditionalGeneration:
        quant = BitsAndBytesConfig(
            load_in_4bit=True,
            bnb_4bit_quant_type="nf4",
            bnb_4bit_compute_dtype=torch.float16,
        )

        model = LlavaForConditionalGeneration.from_pretrained(
            model_id,
            device_map="auto",
            dtype=torch.float16,
            quantization_config=quant,
        )

        return model

    def _load_processor(self) -> LlavaProcessor:
        return LlavaProcessor.from_pretrained(self.model_id)

    def generate_target_detection(
        self, target: str, sample: VideoFrame
    ) -> TargetDetection:
        response = self._process_frame(frame=sample.frame, target=target)
        parsed = self._parse_model_response(model_response=response)
        return TargetDetection(
            description=parsed["description"], ts=sample.ts, exists=parsed["exists"]
        )

    def _process_frame(self, frame: np.ndarray, target: str) -> str:
        inputs = self.processor(
            text=self.PROMPT,
            images=frame,
            return_tensors="pt",
        ).to(self.model.device, self.model.dtype)

        output = self.model.generate(**inputs, max_new_tokens=200, do_sample=False)
        model_response = self.processor.decode(output[0], skip_special_tokens=True)

        return model_response

    def _parse_model_response(self, model_response: str) -> dict:
        match = re.search(r"(?<=assistant\n)\{[\s\S]*\}", model_response)
        if not match:
            return {"exists": False, "description": None}
        return json.loads(match.group(0))
