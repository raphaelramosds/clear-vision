import torch
from transformers import (
    BitsAndBytesConfig,
    LlavaForConditionalGeneration,
    LlavaProcessor,
)

from clear_vision.interfaces.models import HFChatbotModelInterface


class LlavaInterleaveQwenModel(HFChatbotModelInterface):

    def __init__(self, model_id):
        super().__init__(model_id=model_id)
        self.processor = self._load_processor()

    def _load_model(self, model_id) -> LlavaForConditionalGeneration:
        quant = BitsAndBytesConfig(
            load_in_4bit=True,
            bnb_4bit_quant_type="nf4",
            bnb_4bit_compute_dtype=torch.float16,
        )

        return LlavaForConditionalGeneration.from_pretrained(
            model_id,
            device_map="auto",
            dtype=torch.float16,
            quantization_config=quant,
        )

    def _load_processor(self) -> LlavaProcessor:
        return LlavaProcessor.from_pretrained(self.model_id)
