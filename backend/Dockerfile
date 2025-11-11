ARG APP_DIR="/app"
ARG VENV_DIR="${APP_DIR}/.venv"

FROM python:3.12-slim AS build

ARG APP_DIR
ARG VENV_DIR
ENV PATH="${VENV_DIR}/bin:${PATH}"

RUN apt-get update && apt-get install -y --no-install-recommends \
    libgl1 \
    libglib2.0-0 \
    gcc \
    && rm -rf /var/lib/apt/lists/*

WORKDIR ${APP_DIR}

COPY pyproject.toml poetry.lock ./
COPY clear_vision ./clear_vision/

RUN python3 -m venv ${VENV_DIR}
RUN ${VENV_DIR}/bin/pip install --no-cache-dir -U pip setuptools poetry urllib3
RUN ${VENV_DIR}/bin/poetry install --no-root && \
    rm -rf /root/.cache/pip /root/.cache/pypoetry && \
    find . -type d -name "__pycache__" -exec rm -rf {} +

FROM python:3.12-slim AS production

ARG APP_DIR
ARG VENV_DIR
ENV PATH="${VENV_DIR}/bin:${PATH}"

RUN apt-get update && apt-get install -y --no-install-recommends \
    libgl1 \
    libglib2.0-0 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR ${APP_DIR}

COPY --from=build ${APP_DIR}/clear_vision ./clear_vision
COPY --from=build ${VENV_DIR} ${VENV_DIR}
COPY --from=build ${APP_DIR}/pyproject.toml ./

EXPOSE 4000

CMD ["uvicorn", "clear_vision.entrypoints.api.main:app", "--host", "0.0.0.0", "--port", "4000"]
