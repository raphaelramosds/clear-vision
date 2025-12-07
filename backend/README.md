# Clear Vision Backend

## Llava model

The Current version of Clear Vision API uses this model

- [LLaVA Interleave Model Card](https://huggingface.co/llava-hf/llava-interleave-qwen-0.5b-hf)

Before running the API, please download this model with

```bash
huggingface-cli download llava-hf/llava-interleave-qwen-0.5b-hf \
    --local-dir clear_vision/infra/models/llava-hf-llava-interleave-qwen-0.5b-hf \
    --local-dir-use-symlinks False
```