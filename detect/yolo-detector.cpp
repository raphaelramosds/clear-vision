#include "include/yolo-detector.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>

YOLODetector::YOLODetector()
    : confidenceThreshold(0.5f),
      nmsThreshold(0.4f),
      inputWidth(640),
      inputHeight(640) {}

bool YOLODetector::loadModel(const std::string& modelPath,
                              const std::string& configPath,
                              const std::string& classNamesPath) {
    try {
        // Load class names
        if (!classNamesPath.empty()) {
            loadClassNames(classNamesPath);
        }

        // Load YOLO model
        if (modelPath.find(".onnx") != std::string::npos) {
            // ONNX format
            net = cv::dnn::readNetFromONNX(modelPath);
        } else if (!configPath.empty() && modelPath.find(".weights") != std::string::npos) {
            // Darknet format (.weights + .cfg)
            net = cv::dnn::readNetFromDarknet(configPath, modelPath);
        } else {
            std::cerr << "Unsupported model format. Use .onnx or .weights + .cfg" << std::endl;
            return false;
        }

        if (net.empty()) {
            std::cerr << "Failed to load YOLO model from: " << modelPath << std::endl;
            return false;
        }

        // FIXME CMAKE IS NOT INCLUDING CUDA ON BUILD PROCESS EVEN WITH THE FLAGS ENABLED

        // net.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
        // net.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);

        net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
        net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);

        // std::cout << cv::getBuildInformation() << std::endl;

        std::cout << "YOLO model loaded successfully" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading model: " << e.what() << std::endl;
        return false;
    }
}

bool YOLODetector::loadClassNames(const std::string& classNamesPath) {
    std::ifstream file(classNamesPath);
    if (!file.is_open()) {
        std::cerr << "Cannot open class names file: " << classNamesPath << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty()) {
            classNames.push_back(line);
        }
    }
    file.close();
    std::cout << "Loaded " << classNames.size() << " class names" << std::endl;
    return true;
}

std::vector<Detection> YOLODetector::detectObjects(cv::Mat& frame) {
    std::vector<Detection> detections;

    if (net.empty()) {
        std::cerr << "Model not loaded" << std::endl;
        return detections;
    }

    // Prepare blob from frame
    cv::Mat blob = cv::dnn::blobFromImage(
        frame, 1.0 / 255.0,
        cv::Size(inputWidth, inputHeight),
        cv::Scalar(), true, false
    );

    net.setInput(blob);
    std::vector<cv::Mat> outputs;
    net.forward(outputs, net.getUnconnectedOutLayersNames());

    // Process outputs
    std::vector<int> classIds;
    std::vector<float> confidences;
    std::vector<cv::Rect> boxes;

    float xScale = frame.cols / static_cast<float>(inputWidth);
    float yScale = frame.rows / static_cast<float>(inputHeight);

    for (const auto& output : outputs) {
        if (output.dims == 2) {
            // For models outputting [N, 85] format (80 classes + 4 coords + confidence)
            for (int i = 0; i < output.rows; ++i) {
                float confidence = output.at<float>(i, 4);

                if (confidence >= confidenceThreshold) {
                    float x = output.at<float>(i, 0) * xScale;
                    float y = output.at<float>(i, 1) * yScale;
                    float w = output.at<float>(i, 2) * xScale;
                    float h = output.at<float>(i, 3) * yScale;

                    int classId = 0;
                    float maxProb = 0.0f;

                    // Find class with max probability
                    for (int c = 5; c < output.cols; ++c) {
                        float prob = output.at<float>(i, c);
                        if (prob > maxProb) {
                            maxProb = prob;
                            classId = c - 5;
                        }
                    }

                    if (maxProb >= confidenceThreshold) {
                        boxes.push_back(cv::Rect(
                            static_cast<int>(x - w / 2),
                            static_cast<int>(y - h / 2),
                            static_cast<int>(w),
                            static_cast<int>(h)
                        ));
                        confidences.push_back(confidence * maxProb);
                        classIds.push_back(classId);
                    }
                }
            }
        }
    }

    // Apply Non-Maximum Suppression
    std::vector<int> indices;
    cv::dnn::NMSBoxes(boxes, confidences, confidenceThreshold, nmsThreshold, indices);

    // Create detection results
    for (int idx : indices) {
        Detection det;
        det.x = boxes[idx].x;
        det.y = boxes[idx].y;
        det.w = boxes[idx].width;
        det.h = boxes[idx].height;
        det.confidence = confidences[idx];
        det.classId = classIds[idx];
        det.className = getClassName(classIds[idx]);
        detections.push_back(det);
    }

    return detections;
}

void YOLODetector::drawDetections(cv::Mat& frame, const std::vector<Detection>& detections) {
    for (const auto& det : detections) {
        // Draw bounding box
        cv::rectangle(frame, cv::Point(det.x, det.y),
                     cv::Point(det.x + det.w, det.y + det.h),
                     cv::Scalar(0, 255, 0), 2);

        // Draw label with confidence
        std::ostringstream label;
        label << det.className << ": " << std::fixed << std::setprecision(2) 
              << (det.confidence * 100) << "%";
        
        int baseLine = 0;
        cv::Size textSize = cv::getTextSize(label.str(), 
                                           cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
        
        cv::rectangle(frame, cv::Point(det.x, det.y - textSize.height - 5),
                     cv::Point(det.x + textSize.width, det.y),
                     cv::Scalar(0, 255, 0), cv::FILLED);
        cv::putText(frame, label.str(), cv::Point(det.x, det.y - 5),
                   cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0), 1);
    }
}

void YOLODetector::setConfidenceThreshold(float threshold) {
    confidenceThreshold = threshold;
}

void YOLODetector::setNmsThreshold(float threshold) {
    nmsThreshold = threshold;
}

bool YOLODetector::isModelLoaded() const {
    return !net.empty();
}

int YOLODetector::getClassCount() const {
    return static_cast<int>(classNames.size());
}

std::string YOLODetector::getClassName(int classId) const {
    if (classId < 0 || classId >= static_cast<int>(classNames.size())) {
        return "Unknown";
    }
    return classNames[classId];
}