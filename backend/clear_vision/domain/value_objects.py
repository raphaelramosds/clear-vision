from clear_vision.domain.entities import Video


class VideoPath(Video):
    path: str


class VideoUrl(Video):
    url: str
