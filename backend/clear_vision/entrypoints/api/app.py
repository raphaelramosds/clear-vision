from fastapi import FastAPI, UploadFile

app = FastAPI()

# Request Files
# https://fastapi.tiangolo.com/tutorial/request-files/

# POST request method (headers application/form-data and application/x-www-form-urlencoded)
# https://developer.mozilla.org/en-US/docs/Web/HTTP/Reference/Methods/POST

# FastAPI & OpenCV
# https://medium.com/@samakan061/fastapi-opencv-a-comprehensive-guide-to-real-time-face-detection-c12779295f16

@app.post("/uploadfile/")
async def create_upload_file(file: UploadFile):
    return {
        "filename": file.filename,
        "size_mb" : (file.size / 1024) / 1024
    }