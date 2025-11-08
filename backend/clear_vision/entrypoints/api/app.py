from fastapi import FastAPI

from clear_vision.config.containers import Container
from clear_vision.entrypoints.api.routers.videos import videos_router


def create_app() -> FastAPI:
    container = Container()

    app = FastAPI()
    app.container = container
    app.include_router(videos_router)

    return app


app = create_app()
