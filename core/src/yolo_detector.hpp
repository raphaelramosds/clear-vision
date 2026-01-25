#pragma once

#include "yolo_model.hpp"

class YOLODetector
{
private:
    YOLOModel& model;

public:
    YOLODetector(YOLOModel& _model);
    std::vector<cvision::Detection> detectObjects(cv::Mat &frame);
    std::vector<std::vector<cvision::Detection>> detectObjectsBatch(const std::vector<cv::Mat> &frames);
    void drawDetections(cv::Mat &frame, const std::vector<cvision::Detection> &detections);
};