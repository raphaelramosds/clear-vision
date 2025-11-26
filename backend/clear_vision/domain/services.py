from clear_vision.adapters.detectors import LlavaTargetDetector
from clear_vision.domain.entities import InferenceResult
from clear_vision.interfaces.frame_samplers import FrameSamplerInterface
from clear_vision.interfaces.models import ChatbotModelInterface


class VideoService:

    def __init__(
        self, model: ChatbotModelInterface, frame_sampler: FrameSamplerInterface
    ) -> None:
        self.model = model
        self.frame_sampler = frame_sampler

    def add_video(self): ...

    # TODO persist inferences result on a repository
    def view_video_inference_result(self, video_uid: str): ...

    def detect_target_on_video(
        self,
        video_path: str,
        target: str,
    ) -> InferenceResult:
        samples = self.frame_sampler.sample(video_path=video_path)
        detector = LlavaTargetDetector()
        detections = detector.detect_on_frames(video_frames=samples, target=target)
        inference_result = InferenceResult(target=target, detections=detections)

        return inference_result
