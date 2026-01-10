#pragma once

#include <vector>
#include <string>
#include <opencv2/opencv.hpp>

namespace cvision
{
    struct Detection
    {
        int classId;
        float x, y, w, h;
        float confidence;
        std::string className;
    };

    struct Frame
    {
        int frameNumber;
        int ts;
        std::string tsStr;
        std::vector<Detection> detections;
    };

    struct Video
    {
        double fps;
        int totalFrames;
        int width;
        int height;
        std::string videoPath;
    };

    /**
     * @brief Detect objects in each frame of a video file.
     * @param videoPath Path to the video file.
     * @return A vector of Frame structures containing detected objects.
     */
    std::vector<Frame> detect(const std::string &videoPath);

    /**
     * @brief Query a object in frames based on a search string.
     * @param frames A vector of Frame structures containing detected objects.
     * @param query The search string to filter detected objects.
     * @return A vector of Frame structures that match the query.
     */
    std::vector<Frame> query(const std::vector<Frame> &frames, const std::string &q);

    /**
     * @brief Detect and query objects in a video file based on a search string.
     * @param videoPath Path to the video file.
     * @param q The search string to filter detected objects.
     * @return A vector of Frame structures that match the query.
     */
    std::vector<Frame> query(const std::string &videoPath, const std::string &q);

    /**
     * @brief Annotate a frame with bounding boxes and labels for each detection.
     * @param videoPath Path to the video file.
     * @param f The Frame structure to annotate.
     * @param annotated The output annotated frame as a cv::Mat.
     */
    void annotate(const std::string& videoPath, Frame& f, cv::Mat& annotated);
}