from fastapi import APIRouter, WebSocket

healthcheck_router = APIRouter(prefix="/clear-vision/v1")

@healthcheck_router.get("/healthcheck", summary="Healthcheck")
def healthcheck():
    return {"status": "ok"}

@healthcheck_router.websocket("/ws")
async def websocket_endpoint(websocket: WebSocket):
    await websocket.accept()
    while True:
        data = await websocket.receive_text()
        await websocket.send_text(f"Message text was: {data}")