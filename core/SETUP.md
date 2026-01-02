# Clear Vision Detection Model Setup

This document provides step-by-step instructions to set up the environment required to build and run the detection model used in the Clear Vision project.

## JSON for Modern C++

The detection model uses the [JSON for Modern C++](https://github.com/nlohmann/json) library for parsing JSON files.

```bash
sudo apt-get update
sudo apt-get install nlohmann-json3-dev

# Verificar instalação
dpkg -s nlohmann-json3-dev
```

## OpenCV 4.14.0-pre

We will download and compile this shit with CUDA support

CUDA 12 or higher is required to build OpenCV with GPU support. However, GCC version 12 is required to compile CUDA 12 code. Install GCC-12 and G++-12 on your system with the following commands

```bash
sudo apt update
sudo apt install gcc-12 g++-12
```

See the list of GPUs that support CUDA on[CUDA GPU capability](https://developer.nvidia.com/cuda/gpus), and fill the CUDA_ARCH_BIN parameter in the cmake command below accordingly to your GPU Compute Capability.

It is important to install de cuDNN library from NVIDIA to enable GPU acceleration for deep learning tasks.

> In case you want a differente installation configuration, please refer to the [NVIDIA cuDNN installation guide](https://developer.nvidia.com/cudnn-downloads)

```bash
wget https://developer.download.nvidia.com/compute/cudnn/9.17.1/local_installers/cudnn-local-repo-ubuntu2404-9.17.1_1.0-1_amd64.deb
sudo dpkg -i cudnn-local-repo-ubuntu2404-9.17.1_1.0-1_amd64.deb
sudo cp /var/cudnn-local-repo-ubuntu2404-9.17.1/cudnn-*-keyring.gpg /usr/share/keyrings/
sudo apt-get update

# See your CUDA version with
nvcc --version

# To install for CUDA 12, perform the above configuration but install the CUDA 12 specific package:
sudo apt-get -y install cudnn9-cuda-12 libcudnn9-dev-cuda-12

# To install for CUDA 13, perform the above configuration but install the CUDA 13 specific package:
sudo apt-get -y install cudnn9-cuda-13

# Check installation
sudo find /usr -name cudnn.h
```

OpenCV is required to build and run the detection model. Follow the instructions below to download and compile OpenCV on your system.

Beforehand, note the following paths that will be used in the cmake command to build OpenCV with CUDA support

- CUDA_CUDART_LIBRARY can be found with the command `find /usr -name "libcudart.so"`.
- CUDA_INCLUDE_DIRS is usually `/usr/include`.
- CUDA_TOOLKIT_ROOT_DIR is usually `/usr`.

```bash

# On WSL2, the communication between the Linux guest and the Windows host GPU happens through libraries located in 
# /usr/lib/wsl/lib
# Therefore you must add this path to your LD_LIBRARY_PATH environment variable
echo 'export LD_LIBRARY_PATH=/usr/lib/wsl/lib:$LD_LIBRARY_PATH' >> ~/.zshrc

# Add pkgconfig path
echo 'export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/usr/local/lib/pkgconfig'

# Reload .zshrc
source ~/.zshrc

# Get opencv sources
sudo apt update

sudo apt install build-essential cmake git libgtk-3-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev libv4l-dev libxvidcore-dev libx264-dev openexr libatlas-base-dev libopenexr-dev libgstreamer-plugins-base1.0-dev libgstreamer1.0-dev python3-dev python3-numpy libtbbmalloc2 libtbb-dev libjpeg-dev libpng-dev libtiff-dev libdc1394-dev gfortran -y

mkdir ~/opencv_build && cd ~/opencv_build

git clone https://github.com/opencv/opencv.git

git clone https://github.com/opencv/opencv_contrib.git

mkdir -p ~/opencv_build/opencv/build && cd ~/opencv_build/opencv/build

# Build
cmake -D CMAKE_BUILD_TYPE=RELEASE \
-D CMAKE_INSTALL_PREFIX=/usr/local \
-D OPENCV_EXTRA_MODULES_PATH=../../opencv_contrib/modules \
-D WITH_CUDA=ON \
-D ENABLE_FAST_MATH=1 \
-D CUDA_FAST_MATH=1 \
-D WITH_CUBLAS=1 \
-D OPENCV_DNN_CUDA=ON \
-D BUILD_opencv_python3=OFF \
-D WITH_TBB=ON \
-D BUILD_EXAMPLES=OFF \
-D INSTALL_C_EXAMPLES=OFF \
-D INSTALL_PYTHON_EXAMPLES=OFF \
-D CUDA_ARCH_BIN=8.6 \
-D CMAKE_C_COMPILER=gcc-12 \
-D CMAKE_CXX_COMPILER=g++-12 \
-D OPENCV_GENERATE_PKGCONFIG=ON \
-D CUDA_TOOLKIT_ROOT_DIR=/usr \
-D CUDA_INCLUDE_DIRS=/usr/include \
-D CUDA_CUDART_LIBRARY=/usr/lib/x86_64-linux-gnu/libcudart.so \
-D CUDA_NVCC_FLAGS="-allow-unsupported-compiler" \
..

# Install
sudo make install

# Verify installation
ls /usr/local/lib/pkgconfig | grep opencv
```

> More info on [Installing OpenCV on Ubuntu](https://phoenixnap.com/kb/installing-opencv-on-ubuntu)

## Download model

```bash
yolo export model=yolov8n.pt format=onnx opset=12
```

This model is trained to detect objects from the COCO dataset. I already provided the `coco.names` file in this folder, but if you want to create your own, you can find the list of classes in the [COCO dataset website](https://cocodataset.org/#home).

## Debug

Create the following files in the `.vscode` folder at the root of the project to enable building and debugging the detection model in VSCode.

- `tasks.json`: task to build the project in debug mode

```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "build-detect",
            "type": "shell",
            "command": "cd ${workspaceFolder}/detect && cmake -DCMAKE_BUILD_TYPE=Debug -B build && cmake --build build",
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "problemMatcher": [
                "$gcc"
            ],
            "detail": "Build the detect/video-processor project"
        },
        {
            "label": "clean-detect",
            "type": "shell",
            "command": "rm -rf ${workspaceFolder}/detect/build",
            "detail": "Clean build artifacts"
        }
    ]
}

```

Configuration to debug the video-processor application with specific arguments

- `launch.json`
```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug DetectModule",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/detect/build/DetectModule",
            "args": [
                "${workspaceFolder}/detect/video_rua01.mp4",
                "-m",
                "${workspaceFolder}/detect/yolov8n.onnx",
                "-n",
                "${workspaceFolder}/detect/coco.names",
                "-t",
                "0.2",
                "-o",
                "${workspaceFolder}/detect/output.mp4"
            ],
            "cwd": "${workspaceFolder}/detect",
            "preLaunchTask": "build-detect",
            "miDebuggerPath": "/usr/bin/gdb",
            "setupCommands": [
                {
                    "description": "Enable pretty-printing for GDB",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                }
            ]
        }
    ]
}
```

- `c_cpp_properties.json`: include paths for IntelliSense
```json
{
    "configurations": [
        {
            "name": "Linux",
            "includePath": [
                "${workspaceFolder}/**",
                "/usr/local/include/opencv4"
            ],
            "defines": [],
            "compilerPath": "/usr/bin/g++",
            "cStandard": "c17",
            "cppStandard": "c++17",
            "intelliSenseMode": "linux-gcc-x64"
        }
    ],
    "version": 4
}
```