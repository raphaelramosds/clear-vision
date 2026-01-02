#ifndef VIDEO_PROCESSOR_HPP
#define VIDEO_PROCESSOR_HPP

#include "yolo-detector.hpp"
#include <string>

/**
 * Video Processing Class
 * Handles video input/output and processing coordination
 */
class VideoProcessor {
private:
    std::string videoPath;
    std::string outputPath;
    YOLODetector detector;
    bool saveVideo;
    int frameSkip;

public:
    /**
     * Constructor
     * @param video Path to input video
     * @param output Path to output video (optional)
     */
    VideoProcessor(const std::string& video, const std::string& output = "");

    /**
     * Load YOLO model
     * @param modelPath Path to YOLO model
     * @param configPath Path to config file (optional)
     * @param classNamesPath Path to class names file
     * @return true if successful
     */
    bool loadModel(const std::string& modelPath,
                   const std::string& configPath = "",
                   const std::string& classNamesPath = "");

    /**
     * Process video and save annotated output
     * @return true if successful
     */
    bool processVideo();

    /**
     * Process video and export detections to JSON
     * @param jsonOutputPath Path to output JSON file
     * @return true if successful
     */
    bool processVideoToJSON(const std::string& jsonOutputPath);

    /**
     * Set frame skip value
     * @param skip Process every Nth frame (default: 1)
     */
    void setFrameSkip(int skip);

    /**
     * Set confidence threshold
     * @param threshold Confidence threshold (0-1)
     */
    void setConfidenceThreshold(float threshold);

    /**
     * Get detector instance
     * @return Reference to YOLODetector
     */
    YOLODetector& getDetector();
};

#endif // VIDEO_PROCESSOR_HPP