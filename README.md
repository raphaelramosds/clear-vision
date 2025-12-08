# Clear Vision

## Requirements

Python modules

```bash
pip install --no-cache-dir poetry==2.1.4 torch torchvision 
```

Packages to use cv2 modules

```bash
sudo apt-get install -y --no-install-recommends libgl1 libglib2.0-0 gcc
```

## Development environment

```bash
# Run frontend and database
docker compose up -d

# Go to backend directory
cd backend

# Run API locally (without docker)
poetry instal
source ~/.cache/pypoetry/name-of-the-environment-poetry-created/bin/activate
DYNAMODB_ENDPOINT_URL="http://0.0.0.0:8000" \
    uvicorn clear_vision.entrypoints.api.main:app --host 0.0.0.0 --port 4000 --reload
    
# Tests
DYNAMODB_ENDPOINT_URL="http://0.0.0.0:8005" \
   pytest
```