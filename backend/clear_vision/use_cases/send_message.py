from numpy import size
from clear_vision.interfaces.frame_samplers import FrameSamplerInterface
from clear_vision.domain.command import SendMessageCommand


class SendMessageUseCase:

    def __call__(
        self,
        command: SendMessageCommand,
        frame_sampler: FrameSamplerInterface,
    ) -> int:
        num_frames = 6
        path = command.path_to_video
        frames = frame_sampler.sample(path=path, num_frames=num_frames)
        return size(frames)
