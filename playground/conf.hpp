#pragma once

#include <iostream>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string.h>
#include <vector>

struct Config
{
    // Batch
    uint16_t batch_size;
    uint8_t batch_mini_size;
    // Net
    const char *onnx_path;
    uint16_t input_width;
    uint16_t input_height;
    float nms_thr;
    float thr;
    // App
    const char *output_dir;
};

void parse_conf(Config& config);