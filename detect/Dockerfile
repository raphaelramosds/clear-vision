FROM ubuntu:22.04

# Install system dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    wget \
    libopencv-dev \
    libsm6 \
    libxext6 \
    libxrender-dev \
    && rm -rf /var/lib/apt/lists/*

# Optional: Install CUDA for GPU support
# Uncomment the following lines if you want GPU support
# RUN apt-get install -y nvidia-cuda-toolkit

WORKDIR /app

# Copy the video processor code
COPY . .

# Build the application
RUN mkdir -p build && \
    cd build && \
    cmake .. && \
    make

# Create models directory
RUN mkdir -p models

# Download YOLO model and class names
RUN wget -q https://raw.githubusercontent.com/ultralytics/yolov5/master/data/coco.names -O models/coco.names && \
    wget -q https://github.com/ultralytics/assets/releases/download/v0.0.0/yolov8n.onnx -O models/yolov8n.onnx

# Set working directory for processing
WORKDIR /workspace

# Entry point
ENTRYPOINT ["/app/build/video-processor"]
CMD ["--help"]
