# Models for Clear Vision API

## Llava Interleave Qwen 0.5B

The Current version of Clear Vision API uses [LLaVA Interleave Model Card](https://huggingface.co/llava-hf/llava-interleave-qwen-0.5b-hf)

Before running the API, please download this model with

```bash
huggingface-cli download llava-hf/llava-interleave-qwen-0.5b-hf \
    --local-dir clear_vision/infra/models/llava-hf-llava-interleave-qwen-0.5b-hf \
    --local-dir-use-symlinks False
```

I have descontinued the use of this model due to its precision limitations on detect objects on images.

## YOLOv8n

The YOLOv8n model can be used as an alternative to the Llava model for object detection tasks.

Install ultralyitics package

```bash
pip install ultralytics
```
Then download the model with

```bash
yolo export model=yolov8n.pt format=onnx opset=12
```