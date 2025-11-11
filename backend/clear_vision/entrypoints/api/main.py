from fastapi import FastAPI
from fastapi.openapi.utils import get_openapi

from clear_vision.config.containers import Container
from clear_vision.entrypoints.api.routers.videos import videos_router


def create_app() -> FastAPI:
    container = Container()

    app = FastAPI()
    app.container = container
    app.openapi = openapi_settings
    app.include_router(videos_router)

    return app


def openapi_settings():
    if app.openapi_schema:
        return app.openapi_schema
    openapi_schema = get_openapi(
        title="Clear Vision API",
        version="0.1.0",
        summary="API to handle video uploading",
        description="Upload large videos on this API and experiment a little timeout",
        routes=app.routes,
    )
    openapi_schema["info"]["x-logo"] = {
        "url": "https://fastapi.tiangolo.com/img/logo-margin/logo-teal.png"
    }
    app.openapi_schema = openapi_schema
    return app.openapi_schema

app = create_app()
