import torch
import os

from transformers import (
    BitsAndBytesConfig,
    LlavaForConditionalGeneration,
    LlavaProcessor,
)

from clear_vision.interfaces.models import HFChatbotModelInterface


class LlavaInterleaveQwenModel(HFChatbotModelInterface):

    def __init__(self):
        model_id = "llava-hf-llava-interleave-qwen-0.5b-hf"

        # TODO: download this model on Dockerfile build time
        self.local_model_path = os.path.abspath(
            os.path.join(os.path.dirname(__file__), "..", "infra", "models", model_id)
        )

        super().__init__()

        self.processor = self._load_processor()

    def _load_model(self) -> LlavaForConditionalGeneration:
        quant = BitsAndBytesConfig(
            load_in_4bit=True,
            bnb_4bit_quant_type="nf4",
            bnb_4bit_compute_dtype=torch.float16,
        )

        model = LlavaForConditionalGeneration.from_pretrained(
            pretrained_model_name_or_path=self.local_model_path,
            
            # SLOW AS SHIT
            # device_map={"" : "cpu"},

            # FAST !!! (because it will offload on host GPUs)
            device_map="auto",

            dtype=torch.float16,
            quantization_config=quant,
            # low_cpu_mem_usage=False,
            # trust_remote_code=True  
        )

        return model

    def _load_processor(self) -> LlavaProcessor:
        return LlavaProcessor.from_pretrained(
            pretrained_model_name_or_path=self.local_model_path,
            trust_remote_code=True
        )

    @classmethod
    def build_prompt_with_image(cls, context: str):
        return (
            "<|im_start|>user\n"
            "<image>\n\n"
            f"{context}\n"
            "<|im_end|>\n"
            "<|im_start|>assistant\n"
        )
