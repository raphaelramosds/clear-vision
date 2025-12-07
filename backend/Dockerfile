FROM python:3.12-slim AS base

ARG APP_DIR="/app"
WORKDIR ${APP_DIR}

ENV PYTHONUNBUFFERED=1 \
    PIP_NO_CACHE_DIR=1

RUN apt-get update && apt-get install -y --no-install-recommends \
    libgl1 \
    libglib2.0-0 \
    gcc \
    && rm -rf /var/lib/apt/lists/*

COPY pyproject.toml poetry.lock ./

RUN pip install --no-cache-dir poetry==2.1.4

RUN poetry config virtualenvs.create false \
    && poetry install --without dev --no-interaction

RUN pip install torch torchvision --extra-index-url https://download.pytorch.org/whl/cpu

COPY clear_vision ./clear_vision

EXPOSE 4000

CMD ["uvicorn", "clear_vision.entrypoints.api.main:app", "--host", "0.0.0.0", "--port", "4000"]
