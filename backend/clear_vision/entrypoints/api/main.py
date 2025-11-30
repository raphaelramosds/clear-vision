from fastapi import FastAPI
from fastapi.openapi.utils import get_openapi
from fastapi.middleware.cors import CORSMiddleware

from clear_vision.config.containers import Container
from clear_vision.entrypoints.api.routers.videos import videos_router
from clear_vision.entrypoints.api.routers.inferences import inference_router
from clear_vision.entrypoints.api.routers.healthcheck import healthcheck_router


def create_app() -> FastAPI:
    container = Container()

    app = FastAPI()
    app.container = container
    app.openapi = openapi_settings
    app.include_router(healthcheck_router)
    app.include_router(videos_router)
    app.include_router(inference_router)
    app.add_middleware(
        CORSMiddleware,
        allow_origins=[
            "*"
            # "http://localhost:3000",  # frontend Next.js
            # "http://127.0.0.1:3000",  # caso use localhost alternativo
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
        summary="Video processing API with natural-language target detection",
        description=(
            "The Clear Vision API enables uploading and managing videos, as well as running "
            "inference processes to detect targets described using natural language.\n\n"
            "It provides endpoints for:\n"
            "- Uploading and registering videos\n"
            "- Retrieving videos and metadata\n"
            "- Running inference using custom detection models\n"
            "- Fetching inferences by ID or by associated video\n\n"
            "The API is built following clean architecture principles, offering interchangeable "
            "repositories, asynchronous processing support, and straightforward integration with "
            "analysis and monitoring systems."
        ),
        routes=app.routes,
    )

    openapi_schema["info"]["x-logo"] = {
        "url": "https://fastapi.tiangolo.com/img/logo-margin/logo-teal.png"
    }

    app.openapi_schema = openapi_schema
    return app.openapi_schema



app = create_app()
