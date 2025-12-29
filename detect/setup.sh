#!/bin/bash
# Setup script for Clear Vision Video Processor

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "Clear Vision - Video Processor Setup"
echo "===================================="
echo ""

# Check for required tools
check_command() {
    if ! command -v "$1" &> /dev/null; then
        echo "✗ $1 not found. Please install it first."
        return 1
    fi
    echo "✓ $1 found"
    return 0
}

echo "Checking dependencies..."
DEPS_OK=true

check_command cmake || DEPS_OK=false
check_command make || DEPS_OK=false
check_command g++ || check_command clang || { echo "✗ No C++ compiler found"; DEPS_OK=false; }

echo ""

if [ "$DEPS_OK" = false ]; then
    echo "Some dependencies are missing. Please install them first."
    echo ""
    echo "Ubuntu/Debian:"
    echo "  sudo apt-get install build-essential cmake"
    echo ""
    echo "Fedora/RHEL:"
    echo "  sudo dnf install gcc-c++ cmake"
    echo ""
    exit 1
fi

# Check for OpenCV
echo "Checking OpenCV installation..."
if pkg-config --exists opencv4; then
    echo "✓ OpenCV found"
    pkg-config --modversion opencv4
elif pkg-config --exists opencv; then
    echo "✓ OpenCV found"
    pkg-config --modversion opencv
else
    echo "✗ OpenCV not found"
    echo ""
    echo "Please install OpenCV first:"
    echo ""
    echo "Ubuntu/Debian:"
    echo "  sudo apt-get install libopencv-dev"
    echo ""
    echo "Fedora/RHEL:"
    echo "  sudo dnf install opencv-devel"
    echo ""
    echo "Or build from source:"
    echo "  git clone https://github.com/opencv/opencv.git"
    echo "  cd opencv && mkdir build && cd build"
    echo "  cmake -D CMAKE_BUILD_TYPE=Release .."
    echo "  make -j\$(nproc) && sudo make install"
    echo ""
    exit 1
fi

echo ""
echo "===================================="
echo "✓ All dependencies satisfied!"
echo ""
echo "Building video processor..."
cd "$SCRIPT_DIR"
make build

echo ""
echo "===================================="
echo "✓ Build complete!"
echo ""
echo "Next steps:"
echo "1. Download YOLO models:"
echo "   bash download-models.sh"
echo ""
echo "2. Run the video processor:"
echo "   ./build/video-processor video.mp4 -m models/yolov8n.onnx -n models/coco.names"
echo ""
