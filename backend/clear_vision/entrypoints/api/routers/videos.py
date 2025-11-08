from os import path as os_path
from fastapi import APIRouter, Depends, UploadFile
from tempfile import NamedTemporaryFile
from dependency_injector.wiring import inject, Provide

from clear_vision.adapters.strategies import ResolveInMemoryStrategy
from clear_vision.domain.command import SendMessageCommand
from clear_vision.domain.value_objects import VideoPath
from clear_vision.interfaces.frame_samplers import FrameSamplerInterface
from clear_vision.interfaces.models import VideoQAModelInterface
from clear_vision.use_cases.send_message import SendMessageUseCase

videos_router = APIRouter()


@videos_router.post("/upload/")
@inject
def upload(
    video: UploadFile,
    frame_sampler: FrameSamplerInterface = Depends(Provide["frame_sampler"]),
    model: VideoQAModelInterface = Depends(Provide["model"]),
):
    dot_ext = os_path.splitext(video.filename)[1]
    content = video.file.read()
    size_mb = len(content) / (1024 * 1024)

    temp = NamedTemporaryFile(delete=True, suffix=dot_ext)
    temp.write(content)
    temp.flush()

    send_message_use_case = SendMessageUseCase()
    command = SendMessageCommand(
        path_to_video=ResolveInMemoryStrategy().execute(
            video=VideoPath(path=temp.name)
        ),
        user_prompt="In which minute does a cat appear on this video?",
    )

    result = send_message_use_case(
        command=command,
        model=model,
        frame_sampler=frame_sampler,
    )

    temp.close()

    return {
        "result": result,
    }
