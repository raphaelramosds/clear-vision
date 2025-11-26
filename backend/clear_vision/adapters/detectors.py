import re
import json

from clear_vision.adapters.models import LlavaInterleaveQwenModel
from clear_vision.interfaces.detectors import GeneralTargetDetectorInterface


class LlavaTargetDetector(GeneralTargetDetectorInterface):

    def __init__(self):
        self.chatbot_model = LlavaInterleaveQwenModel()

    def process_frame(self, frame, target):

        prompt = self.chatbot_model.build_prompt_with_image(
            """
Analyze the image and search for this target object: "{target}".
Then, return ONLY JSON in the following format:
{{"exists": boolean, "description": string}}
Where:
- "description": must describe ALL visible objects in the image.
- "exists": must be true if the target object is present. Otherwise, it must be false.
""".format(
                target=target
            )
        )

        inputs = self.chatbot_model.processor(
            text=prompt,
            images=frame,
            return_tensors="pt",
        ).to(self.chatbot_model.model.device, self.chatbot_model.model.dtype)

        output = self.chatbot_model.model.generate(
            **inputs, max_new_tokens=200, do_sample=False
        )
        model_response = self.chatbot_model.processor.decode(
            output[0], skip_special_tokens=True
        )

        return model_response

    def parse_model_response(self, model_response) -> dict:
        match = re.search(r"(?<=assistant\n)\{[\s\S]*\}", model_response)
        if not match:
            return {"exists": False, "description": None}

        return json.loads(match.group(0))
