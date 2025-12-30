# Detection model

This folder contains the code and resources related to the detection model used in the Clear Vision project. The detection model is responsible for identifying and localizing objects within images or video frames.

## Debug

Task to build the project in debug mode

```json
// tasks.json

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

```json
// launch.json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug video-processor",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/detect/build/video-processor",
            "args": [
                "${workspaceFolder}/detect/video_rua01.mp4",
                "-m",
                "${workspaceFolder}/detect/yolov8n.onnx",
                "-n",
                "${workspaceFolder}/detect/coco.names",
                "-o",
                "${workspaceFolder}/detect/output.mp4"
            ],
            "cwd": "${workspaceFolder}/detect",
            "preLaunchTask": "build-detect",
            "miDebuggerPath": "/usr/bin/gdb"
        }
    ]
}
```