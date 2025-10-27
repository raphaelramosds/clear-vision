from os import path as os_path

from tempfile import NamedTemporaryFile
from fastapi import FastAPI, UploadFile

from clear_vision.services.send_message import call_send_message

app = FastAPI()


@app.post("/upload/")
async def upload(video: UploadFile):
    dot_ext = os_path.splitext(video.filename)[1]
    # Move content to a temp file
    temp = NamedTemporaryFile(delete=False, suffix=dot_ext, delete_on_close=True)
    with video.file as f:
        temp.write(f.read())

    # Process frames
    # ...

    # Remove temp file
    temp.close()

    return {
        "filename": video.filename,
        "size_mb": (video.size / 1024) / 1024,
        "temp": temp.name,
    }
