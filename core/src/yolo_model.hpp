#pragma once

#include "cvision.hpp"
#include "opencv2/dnn.hpp"

class YOLOModel
{
private:
    cv::dnn::Net net;
    std::vector<std::string> classNames;
    float confidenceThreshold;
    float nmsThreshold;
    int inputWidth;
    int inputHeight;

public:
    YOLOModel(
        std::string &onnxPath,
        std::string &classNamesPath,
        float confThreshold = 0.5,
        float nmsThreshold = 0.4,
        int inputWidth = 640,
        int inputHeight = 640);

    float getConfidenceThreshold() const
    {
        return (float)this->confidenceThreshold;
    };

    void setConfidenceThreshold(float _c_thr)
    {
        this->confidenceThreshold = _c_thr;
    };

    float getNmsThreshold() const
    {
        return (float)this->nmsThreshold;
    };

    void setNmsThreshold(float _nms_thr)
    {
        this->nmsThreshold = _nms_thr;
    };

    cv::dnn::Net &getNet()
    {
        return this->net;
    };

    int getInputWidth() const
    {
        return this->inputWidth;
    };

    int getInputHeight() const
    {
        return this->inputHeight;
    };

    std::string getClassName(int classId) const;
};