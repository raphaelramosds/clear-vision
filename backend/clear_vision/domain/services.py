from clear_vision.domain.entities import Inference
from clear_vision.interfaces.detectors import GeneralTargetDetectorInterface
from clear_vision.interfaces.frame_samplers import FrameSamplerInterface


class InferenceService:

    def __init__(
        self,
        detector: GeneralTargetDetectorInterface,
        frame_sampler: FrameSamplerInterface,
    ) -> None:
        self.detector = detector
        self.frame_sampler = frame_sampler

    # TODO persist inferences result on a repository
    def get_inference_result(self, video_uid: str): ...

    def list_inference_results(self): ...

    def run_video_target_detection(
        self,
        video_path: str,
        target: str,
    ) -> Inference:
        samples = self.frame_sampler.sample(video_path=video_path)
        detections = self.detector.detect_many(video_frames=samples, target=target)
        inference_result = Inference(
            target=target,
            detections=[
                detection for detection in detections if detection.target_exists == True
            ],
        )

        return inference_result
