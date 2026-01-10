#include "yolo_detector.hpp"
#include "levenshtein_search.hpp"
#include <filesystem>
#include <opencv2/opencv.hpp>

std::vector<cvision::Frame> cvision::detect(const std::string &videoPath)
{
    if (!std::filesystem::exists(videoPath))
    {
        throw std::invalid_argument("Video file does not exist: " + videoPath);
    }

    cv::VideoCapture cap(videoPath);
    if (!cap.isOpened())
    {
        throw std::runtime_error("Cannot open video file: " + videoPath);
    }

    unsigned int fps = cap.get(cv::CAP_PROP_FPS);
    unsigned int totalFrames = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_COUNT));
    unsigned int width = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
    unsigned int height = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));
    unsigned int i;

    // Prepare YOLO model and detector
    std::string modelPath = "yolov8x.onnx";
    std::string classNamesPath = "coco.names";
    YOLOModel model = YOLOModel(modelPath, classNamesPath);
    YOLODetector detector = YOLODetector(model);

    // Process frames
    std::vector<cvision::Frame> frames = {};
    cv::Mat frame;

    // TODO: parallelize this loop
    for (i = 0; i < totalFrames; ++i)
    {
        // Read current frame
        cap.read(frame);

        // Get detections for the current frame
        std::vector<Detection> current_detections = detector.detectObjects(frame);

        Frame f;
        f.frameNumber = i;
        f.ts = static_cast<int>(i / fps);
        f.detections = current_detections;
        f.tsStr = cv::format("%02d:%02d:%02d", f.ts / 3600, (f.ts % 3600) / 60, f.ts % 60);

        frames.push_back(f);

        std::cout << "Processed frame " << i + 1 << " / " << totalFrames << "\r" << std::flush;
    }

    cap.release();

    return frames;
}

std::vector<cvision::Frame> cvision::query(const std::vector<cvision::Frame> &frames, const std::string &query)
{
    LevenshteinSearch w2vSearch(frames);
    std::vector<cvision::Frame> results = w2vSearch.search(query, 0.3f);

    if (results.empty())
    {
        std::cout << "No results found for the given query." << std::endl;
        return {};
    }

    std::cout << "Found " << results.size() << " frames matching the query." << std::endl;
    return results;
}

std::vector<cvision::Frame> cvision::query(const std::string &videoPath, const std::string &q)
{
    std::vector<cvision::Frame> frames = cvision::detect(videoPath);
    return cvision::query(frames, q);
}