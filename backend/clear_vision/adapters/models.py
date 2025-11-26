import torch

from transformers import (
    BitsAndBytesConfig,
    LlavaForConditionalGeneration,
    LlavaProcessor,
)

from clear_vision.config.decorators import singleton
from clear_vision.interfaces.models import HFChatbotModelInterface


@singleton
class LlavaInterleaveQwenModel(HFChatbotModelInterface):

    def __init__(self):
        super().__init__(model_id="llava-hf/llava-interleave-qwen-0.5b-hf")
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

    @classmethod
    def build_prompt_with_image(cls, context: str):
        return (
            "<|im_start|>user\n"
            "<image>\n\n"
            f"{context}\n"
            "<|im_end|>\n"
            "<|im_start|>assistant\n"
        )
