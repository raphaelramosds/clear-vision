# Profiling

## gprof

This project can be built with gprof instrumentation enabled. To do so, set the CMake option `CVISION_ENABLE_GPROF` to `ON` when configuring the build:

```bash
cd ${workspaceFolder}/core

# Create build diretory and configure CMake
mkdir build

# Build with gprof enabled
cmake -DCVISION_ENABLE_GPROF=ON -DUSE_OPENMP=OFF -DCMAKE_BUILD_TYPE=Debug -B build

# Generate executable
cmake --build build

./build/cvision ../sample_video.mp4 -m ../yolov8x.onnx -n ../coco.names
```

When this option is enabled, the compiler and linker flags `-pg` will be added to the build process. This allows the generation of profiling data when the application is executed.

Then interpret the profiling data with gprof

```bash
gprof --ignore-non-functions build/cvision > profiling/profiling.txt
```

## perf

Install `perf` on Arch Linux

```bash
sudo pacman -Sy perf
```

Run the application with `perf` recording

```bash
perf record -g ./build/cvision video_rua01.mp4 -m yolov8n.onnx -n coco.names
```