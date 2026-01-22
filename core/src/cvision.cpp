#include "yolo_detector.hpp"
#include "levenshtein_search.hpp"
#include <filesystem>
#include <opencv2/opencv.hpp>
// #include <tbb/parallel_for.h>
#include <utility>

std::string modelPath = "yolov8x.onnx";
std::string classNamesPath = "coco.names";

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

    cap.release();

    // Process frames
    std::vector<cvision::Frame> frames(totalFrames);

    // TODO race condition if multiple threads inputs to the same net
    // TODO large memory consumption if each thread has its own net copy
    // TODO if a critical section is used, each thread will wait for the net access, losing parallelism benefits

    // tbb::parallel_for(
    //     tbb::blocked_range<unsigned int>(0, totalFrames),
    //     [&](const tbb::blocked_range<unsigned int> &r)
    //     {
    //         YOLOModel local_model = YOLOModel(modelPath, classNamesPath);
    //         YOLODetector local_detector = YOLODetector(local_model);
    //         cv::VideoCapture local_cap(videoPath);
    //         cv::Mat local_frame;

    //         for (unsigned int j = r.begin(); j != r.end(); ++j)
    //         {
    //             // Read current frame
    //             local_cap.set(cv::CAP_PROP_POS_FRAMES, j);
    //             local_cap.read(local_frame);

    //             // Get detections for the current frame
    //             std::vector<Detection> current_detections = local_detector.detectObjects(local_frame);

    //             Frame f;
    //             f.frameNumber = j;
    //             f.ts = static_cast<int>(j / fps);
    //             f.detections = current_detections;
    //             f.tsStr = cv::format("%02d:%02d:%02d", f.ts / 3600, (f.ts % 3600) / 60, f.ts % 60);

    //             frames[j] = std::move(f);
    //         }
    //         local_cap.release();
    //     },
    //     tbb::static_partitioner());

    // SEQUENTIAL PROCESSING //
    YOLOModel model = YOLOModel(modelPath, classNamesPath);
    YOLODetector detector = YOLODetector(model);
    cv::VideoCapture cap2(videoPath);
    cv::Mat frame;
    for (i = 0; i < totalFrames; ++i)
    {
        // Read current frame
        cap2.read(frame);

        // Get detections for the current frame
        std::vector<Detection> current_detections = detector.detectObjects(frame);

        Frame f;
        f.frameNumber = i;
        f.ts = static_cast<int>(i / fps);
        f.detections = current_detections;
        f.tsStr = cv::format("%02d:%02d:%02d", f.ts / 3600, (f.ts % 3600) / 60, f.ts % 60);

        frames[i] = std::move(f);
        std::cout << "Processed frame " << i + 1 << " / " << totalFrames << "\r" << std::flush;
    }

    cap2.release();

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

void cvision::annotate(const std::string &videoPath, cvision::Frame &f, cv::Mat &annotated)
{
    cv::Mat frame;
    cv::VideoCapture cap(videoPath);

    YOLOModel model = YOLOModel(modelPath, classNamesPath);
    YOLODetector detector = YOLODetector(model);

    cap.set(cv::CAP_PROP_POS_FRAMES, f.frameNumber);
    cap.read(frame);
    detector.drawDetections(frame, f.detections);
    cap.release();

    frame.assignTo(annotated);
}