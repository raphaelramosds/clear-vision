# Clear Vision Video Processor

A high-performance C++ application for video object detection using YOLO models. Processes video files, extracts frames, applies YOLO-based object detection, and optionally saves the annotated video or detection data.

## Features

- **Video Reading**: Efficient video reading and frame extraction using OpenCV
- **YOLO Object Detection**: Support for both ONNX and Darknet (.weights + .cfg) YOLO models
- **GPU Acceleration**: CUDA support for faster inference (falls back to CPU if unavailable)
- **Flexible Output**: 
  - Annotated video output with bounding boxes and labels
  - JSON detection export for integration with other systems
- **Frame Skipping**: Process every Nth frame for faster processing
- **Confidence Thresholding**: Adjustable confidence threshold for detections
- **NMS Processing**: Non-Maximum Suppression to eliminate duplicate detections

## Requirements

### System Dependencies

- **CMake** >= 3.10
- **C++17** compatible compiler (GCC, Clang, or MSVC)
- **OpenCV** >= 4.5.0 (with DNN module)
- **CUDA** (optional, for GPU acceleration)

### Installation on Linux

#### Ubuntu/Debian:
```bash
# Install build tools
sudo apt-get update
sudo apt-get install -y build-essential cmake git

# Install OpenCV with DNN support
sudo apt-get install -y libopencv-dev

# Optional: Install CUDA for GPU support
# Follow: https://developer.nvidia.com/cuda-downloads
```

#### Fedora/RHEL:
```bash
sudo dnf install -y gcc-c++ cmake opencv-devel

# Optional: CUDA
# Follow: https://developer.nvidia.com/cuda-downloads
```

#### macOS:
```bash
brew install cmake opencv

# Optional: CUDA (limited support on macOS)
```

### Building OpenCV from Source (Recommended for DNN module)

```bash
git clone https://github.com/opencv/opencv.git
cd opencv
mkdir build && cd build
cmake -D CMAKE_BUILD_TYPE=Release \
      -D CMAKE_INSTALL_PREFIX=/usr/local \
      -D WITH_CUDA=ON \
      -D CUDA_ARCH_BIN=YOUR_GPU_COMPUTE_CAPABILITY \
      ..
make -j$(nproc)
sudo make install
```

## Building

### Using Make (Recommended)

```bash
cd video
make build
```

### Using CMake Directly

```bash
cd video
mkdir build
cd build
cmake ..
make
```

### Build Options

```bash
# Build with verbose output
cd build && cmake --build . --verbose

# Build with specific number of threads
make -j4
```

## Usage

### Basic Usage

```bash
./video-processor <video_path> -m <model_path> -n <class_names_path>
```

### Options

| Option | Alias | Description |
|--------|-------|-------------|
| `-m, --model` | Required | Path to YOLO model (.onnx or .weights) |
| `-c, --config` | Optional | Path to YOLO config (.cfg) for .weights models |
| `-n, --names` | Optional | Path to class names file (one class per line) |
| `-o, --output` | Optional | Output video path (MP4 format) |
| `-j, --json` | Optional | Save detections to JSON file |
| `-s, --skip` | Optional | Process every Nth frame (default: 1) |
| `-t, --threshold` | Optional | Confidence threshold 0-1 (default: 0.5) |
| `-h, --help` | | Show help message |

### Examples

#### Process Video with ONNX Model and Save Annotated Output

```bash
./video-processor input.mp4 \
  -m yolov8n.onnx \
  -n coco.names \
  -o output_annotated.mp4
```

#### Process Video and Export Detections to JSON

```bash
./video-processor input.mp4 \
  -m yolov8n.onnx \
  -n coco.names \
  -j detections.json
```

#### Fast Processing - Every 5th Frame with Lower Threshold

```bash
./video-processor input.mp4 \
  -m yolov8n.onnx \
  -n coco.names \
  -s 5 \
  -t 0.3
```

#### Using Darknet Format Model

```bash
./video-processor input.mp4 \
  -m weights.weights \
  -c config.cfg \
  -n classes.txt \
  -o output.mp4
```

## YOLO Model Setup

### Download YOLOv8 ONNX Models

```bash
# YOLOv8 Nano (fastest)
wget https://github.com/ultralytics/assets/releases/download/v0.0.0/yolov8n.onnx

# YOLOv8 Small
wget https://github.com/ultralytics/assets/releases/download/v0.0.0/yolov8s.onnx

# YOLOv8 Medium
wget https://github.com/ultralytics/assets/releases/download/v0.0.0/yolov8m.onnx
```

### COCO Class Names

```bash
cat > coco.names << EOF
person
bicycle
car
motorbike
aeroplane
bus
train
truck
boat
EOF
# ... (80 total classes)
```

Or download pre-made:
```bash
wget https://raw.githubusercontent.com/ultralytics/yolov5/master/data/coco.names
```

## Output Formats

### Annotated Video Output

- Format: MP4 (H.264)
- Contains bounding boxes with class names and confidence scores
- Same resolution and FPS as input video

### JSON Output Format

```json
[
  {
    "frame_number": 1,
    "detections": [
      {
        "class_name": "person",
        "confidence": 0.95,
        "x": 100,
        "y": 150,
        "width": 200,
        "height": 300
      },
      {
        "class_name": "car",
        "confidence": 0.87,
        "x": 400,
        "y": 200,
        "width": 250,
        "height": 180
      }
    ]
  },
  {
    "frame_number": 2,
    "detections": []
  }
]
```

## Performance Tips

1. **GPU Acceleration**: Build with CUDA support for 5-20x faster processing
2. **Frame Skipping**: Use `-s 2` or higher for real-time analysis of long videos
3. **Model Size**: Use YOLOv8 Nano (`yolov8n.onnx`) for fastest inference
4. **Resolution**: Process at lower resolution for faster inference
5. **Batch Processing**: For multiple videos, process sequentially to optimize memory usage

## Integration with Backend

This video processor can be integrated with the Clear Vision backend API:

```bash
# Process video and save detections
./video-processor video.mp4 \
  -m yolov8n.onnx \
  -n coco.names \
  -j detections.json \
  -o annotated.mp4

# Then send detections and video to backend API
curl -X POST http://localhost:8000/api/inferences \
  -F "video=@annotated.mp4" \
  -F "detections=@detections.json"
```

## Troubleshooting

### OpenCV not found
```bash
# Install development headers
sudo apt-get install libopencv-dev

# Or build from source
git clone https://github.com/opencv/opencv.git
cd opencv && mkdir build && cd build
cmake -D CMAKE_BUILD_TYPE=Release .. && make && sudo make install
```

### CUDA-related errors
```bash
# Fall back to CPU (automatically done if CUDA unavailable)
# Or explicitly use CPU in code

# Check CUDA installation
nvidia-smi
```

### Video codec issues
```bash
# Install ffmpeg for codec support
sudo apt-get install ffmpeg libavcodec-extra
```

### Out of memory errors
- Use frame skipping: `-s 5`
- Use smaller model: `yolov8n.onnx` instead of larger variants
- Reduce output video quality

## Architecture

### Main Components

1. **YOLODetector Class**
   - Handles model loading (ONNX and Darknet formats)
   - Performs inference on frames
   - Applies NMS post-processing
   - Draws annotations

2. **VideoProcessor Class**
   - Manages video reading and writing
   - Coordinates detection and output
   - Handles JSON export
   - Provides progress tracking

3. **Detection Structure**
   - Stores bounding box coordinates
   - Maintains confidence scores
   - Links to class information

## Performance Metrics

Typical performance on NVIDIA GeForce RTX 3070:
- **YOLOv8n**: ~100-150 FPS
- **YOLOv8s**: ~50-80 FPS
- **YOLOv8m**: ~25-40 FPS

On CPU (Intel i7-11700K):
- **YOLOv8n**: ~10-15 FPS
- **YOLOv8s**: ~5-8 FPS

## Future Enhancements

- [ ] Multi-GPU support
- [ ] Tracking across frames
- [ ] Custom YOLO model training pipeline
- [ ] Real-time webcam processing
- [ ] Video streaming support
- [ ] Batch processing of multiple videos
- [ ] Advanced filtering and post-processing

## License

Part of the Clear Vision project

## Support

For issues or questions, create an issue in the main Clear Vision repository.
