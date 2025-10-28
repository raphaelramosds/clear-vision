from pydantic import BaseModel

class Command(BaseModel): ...


class SendMessageCommand(Command):
    path_to_video: str
    user_prompt: str