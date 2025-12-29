#!/bin/bash
# Script to download YOLO models and class names

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
MODELS_DIR="${SCRIPT_DIR}/models"

# Create models directory
mkdir -p "$MODELS_DIR"

echo "Clear Vision - YOLO Model Downloader"
echo "===================================="
echo ""

# Function to download with progress
download_file() {
    local url=$1
    local output=$2
    echo "Downloading: $output"
    
    if command -v wget &> /dev/null; then
        wget -q --show-progress "$url" -O "$output"
    elif command -v curl &> /dev/null; then
        curl -L -o "$output" "$url"
    else
        echo "Error: Neither wget nor curl found. Please install one of them."
        exit 1
    fi
    
    echo "✓ Downloaded: $output"
}

# Download COCO class names
echo "1. Downloading COCO class names..."
download_file \
    "https://raw.githubusercontent.com/ultralytics/yolov5/master/data/coco.names" \
    "$MODELS_DIR/coco.names"

# Download YOLOv8 Nano ONNX
echo ""
echo "2. Downloading YOLOv8 Nano (fastest, recommended for real-time)..."
download_file \
    "https://github.com/ultralytics/assets/releases/download/v0.0.0/yolov8n.onnx" \
    "$MODELS_DIR/yolov8n.onnx"

# Optional: Download other models
echo ""
echo "Optional: Download additional models?"
read -p "Download YOLOv8 Small? (y/n) " -n 1 -r
echo
if [[ $REPLY =~ ^[Yy]$ ]]; then
    download_file \
        "https://github.com/ultralytics/assets/releases/download/v0.0.0/yolov8s.onnx" \
        "$MODELS_DIR/yolov8s.onnx"
fi

read -p "Download YOLOv8 Medium? (y/n) " -n 1 -r
echo
if [[ $REPLY =~ ^[Yy]$ ]]; then
    download_file \
        "https://github.com/ultralytics/assets/releases/download/v0.0.0/yolov8m.onnx" \
        "$MODELS_DIR/yolov8m.onnx"
fi

echo ""
echo "===================================="
echo "✓ All selected models downloaded!"
echo ""
echo "Models are located in: $MODELS_DIR"
echo ""
echo "Usage example:"
echo "  ./video-processor video.mp4 -m $MODELS_DIR/yolov8n.onnx -n $MODELS_DIR/coco.names -o output.mp4"
