import uuid
import requests
from clear_vision.domain.value_objects import VideoPath, VideoUrl
from clear_vision.interfaces.strategies import ResolvePathStrategyInterface


class ResolveInMemoryStrategy(ResolvePathStrategyInterface):

    def execute(self, video: VideoPath):
        return video.path


class ResolveUrlStrategy(ResolvePathStrategyInterface):

    def execute(self, video: VideoUrl):
        response = requests.get(video.url)
        path_id = str(uuid.uuid4())
        path = f"./backend/public/{path_id}.mp4"

        with open(path, "wb") as f:
            f.write(response.content)

        return path
