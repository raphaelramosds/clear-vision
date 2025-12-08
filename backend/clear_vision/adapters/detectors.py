import re
import json
import typing as t

import numpy as np

from clear_vision.config.logger import get_logger
from clear_vision.domain.value_objects import TargetDetection
from clear_vision.adapters.models import LlavaInterleaveQwenModel
from clear_vision.domain.value_objects import VideoFrame
from clear_vision.interfaces.detectors import GeneralTargetDetectorInterface
from clear_vision.interfaces.frame_samplers import FrameSamplerInterface

logger = get_logger(__name__)

class LlavaTargetDetector(GeneralTargetDetectorInterface):
    DETECTOR_ASSISTANT_PROMPT = """
    Analyze the image and search for this target object: "{target}".
    Then, return ONLY JSON in the following format:
    {{"exists": boolean}}
    Where:
    - "exists": must be True if the target object is present. Otherwise, it must be False.
    """

    def __init__(self, frame_sampler: FrameSamplerInterface):
        self._chatbot_model = LlavaInterleaveQwenModel()
        self._frame_sampler = frame_sampler

    def __call__(self, video_path, target):
        samples = self._frame_sampler.sample(video_path=video_path)
        detections = self._get_detections(video_frames=samples, target=target)
        return detections

    def _get_detections(self, video_frames: t.List[VideoFrame], target: str) -> t.List[TargetDetection]:
        def get_detection(video_frame: VideoFrame) -> TargetDetection:
            response = self._run_detection(frame=video_frame.data, target=target)
            parsed = self._parse_model_response(model_response=response)
            return TargetDetection(
                ts=video_frame.ts,
                target_exists=parsed["exists"],
            )

        return [
            get_detection(video_frame=video_frame)
            for video_frame in video_frames
        ]

    def _run_detection(self, frame: np.ndarray, target: str) -> str:
        prompt = self._chatbot_model.build_prompt_with_image(
            self.DETECTOR_ASSISTANT_PROMPT.format(target=target)
        )
        inputs = self._chatbot_model.processor(
            text=prompt,
            images=frame,
            return_tensors="pt",
        ).to(self._chatbot_model.model.device, self._chatbot_model.model.dtype)

        output = self._chatbot_model.model.generate(
            **inputs, max_new_tokens=200, do_sample=False
        )
        model_response = self._chatbot_model.processor.decode(
            output[0], skip_special_tokens=True
        )

        # logger.info(model_response)

        return model_response

    def _parse_model_response(self, model_response: str) -> dict:
        match = re.search(r"(?<=assistant\n)\{[\s\S]*\}", model_response)
        if not match:
            return {"exists": False}

        return json.loads(match.group(0))
