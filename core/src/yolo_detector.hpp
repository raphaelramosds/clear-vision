#pragma once

#include "yolo_model.hpp"

class YOLODetector
{
private:
    YOLOModel& model;

public:
    YOLODetector(YOLOModel& _model);
    std::vector<cvision::Detection> detectObjects(cv::Mat &frame);
    void drawDetections(cv::Mat &frame, const std::vector<cvision::Detection> &detections);
};