from clear_vision.interfaces.frame_samplers import FrameSamplerInterface
from clear_vision.domain.command import SendMessageCommand
from clear_vision.interfaces.models import VideoQAModelInterface


class SendMessageUseCase:

    def __call__(
        self,
        command: SendMessageCommand,
        model: VideoQAModelInterface,
        frame_sampler: FrameSamplerInterface,
    ) -> str:
        num_frames = 6
        path = command.path_to_video
        frames = frame_sampler.sample(path=path, num_frames=num_frames)
        return model.generate(command.user_prompt, num_frames, frames)
