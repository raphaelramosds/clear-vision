#ifndef YOLO_DETECTOR_HPP
#define YOLO_DETECTOR_HPP

#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

/**
 * Structure to hold detection results
 */
struct Detection {
    int ts = 0;                 // Timestamp or frame number
    float x, y, w, h;           // Bounding box coordinates and dimensions
    float confidence;           // Confidence score
    int classId;                 // Class ID
    std::string className;       // Class name
    std::string tsStr;           // Timestamp as string
};

/**
 * YOLO Object Detector Class
 * Handles model loading, inference, and post-processing
 */
class YOLODetector {
private:
    cv::dnn::Net net;
    std::vector<std::string> classNames;
    float confidenceThreshold;
    float nmsThreshold;
    int inputWidth;
    int inputHeight;

    /**
     * Load class names from file
     * @param classNamesPath Path to class names file
     * @return true if successful
     */
    bool loadClassNames(const std::string& classNamesPath);

public:
    /**
     * Default constructor
     */
    YOLODetector();

    /**
     * Load YOLO model and class names
     * @param modelPath Path to YOLO .onnx or .weights file
     * @param configPath Path to YOLO config file (if using .weights)
     * @param classNamesPath Path to class names file
     * @return true if loading successful
     */
    bool loadModel(const std::string& modelPath,
                   const std::string& configPath = "",
                   const std::string& classNamesPath = "");

    /**
     * Detect objects in a frame
     * @param frame Input image
     * @return Vector of detected objects
     */
    std::vector<Detection> detectObjects(cv::Mat& frame);

    /**
     * Draw detections on frame
     * @param frame Output image to draw on
     * @param detections Vector of detections to draw
     */
    void drawDetections(cv::Mat& frame, const std::vector<Detection>& detections);

    /**
     * Set confidence threshold for detections
     * @param threshold Confidence threshold (0-1)
     */
    void setConfidenceThreshold(float threshold);

    /**
     * Set NMS threshold for post-processing
     * @param threshold NMS threshold (0-1)
     */
    void setNmsThreshold(float threshold);

    /**
     * Check if model is loaded
     * @return true if model is valid
     */
    bool isModelLoaded() const;

    /**
     * Get number of loaded classes
     * @return Number of classes
     */
    int getClassCount() const;

    /**
     * Get class name by ID
     * @param classId Class ID
     * @return Class name or "Unknown"
     */
    std::string getClassName(int classId) const;
};

#endif // YOLO_DETECTOR_HPP