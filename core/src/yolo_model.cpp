#include "yolo_model.hpp"

#include <iostream>
#include <fstream>

YOLOModel::YOLOModel(
    std::string &onnxPath,
    std::string &classNamesPath,
    float confThreshold,
    float nmsThreshold,
    int inputWidth,
    int inputHeight)
    : confidenceThreshold(confThreshold),
      nmsThreshold(nmsThreshold),
      inputWidth(inputWidth),
      inputHeight(inputHeight)
{
    try
    {
        // Load class names
        if (!classNamesPath.empty())
        {
            std::ifstream file(classNamesPath);
            if (!file.is_open())
            {
                std::cerr << "Cannot open class names file: " << classNamesPath << std::endl;
                throw std::runtime_error("Cannot open class names file");
            }

            std::string line;
            while (std::getline(file, line))
            {
                if (!line.empty())
                {
                    classNames.push_back(line);
                }
            }
            file.close();
            std::cout << "Loaded " << classNames.size() << " class names" << std::endl;
        }

        // Load YOLO model
        if (onnxPath.find(".onnx") != std::string::npos)
        {
            // ONNX format
            net = cv::dnn::readNetFromONNX(onnxPath);
        }
        else
        {
            std::cerr << "Unsupported model format. Use .onnx or .weights + .cfg" << std::endl;
            throw std::runtime_error("Unsupported model format");
        }

        if (net.empty())
        {
            std::cerr << "Failed to load YOLO model from: " << onnxPath << std::endl;
            throw std::runtime_error("Failed to load model");
        }

        // Try to enable CUDA acceleration with fallback to CPU
        bool cudaSuccess = false;
        try
        {
            net.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
            net.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA_FP16);
            std::cout << "Using CUDA backend/target for YOLO inference" << std::endl;
            cudaSuccess = true;
        }
        catch (const std::exception &e)
        {
            std::cerr << "CUDA backend failed (" << e.what() << ") - falling back to CPU" << std::endl;
            net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
            net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
        }

        std::cout << "YOLO model loaded successfully" << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error loading model: " << e.what() << std::endl;
        exit(-1);
    }
}

std::string YOLOModel::getClassName(int classId) const
{
    if (classId < 0 || classId >= static_cast<int>(classNames.size()))
    {
        return "Unknown";
    }
    return classNames[classId];
}