from fastapi import FastAPI
from fastapi.openapi.utils import get_openapi
from fastapi.middleware.cors import CORSMiddleware

from clear_vision.config.containers import Container
from clear_vision.entrypoints.api.routers.videos import videos_router


def create_app() -> FastAPI:
    container = Container()

    app = FastAPI()
    app.container = container
    app.openapi = openapi_settings
    app.include_router(videos_router)

    # 🟢 Configuração de CORS
    app.add_middleware(
        CORSMiddleware,
        allow_origins=[
            "http://localhost:3000",  # frontend Next.js
            "http://127.0.0.1:3000",  # caso use localhost alternativo
        ],
        allow_credentials=True,
        allow_methods=["*"],
        allow_headers=["*"],
    )

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
