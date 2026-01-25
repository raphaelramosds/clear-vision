#include "yolo_detector.hpp"
#include "levenshtein_search.hpp"
#include <filesystem>
#include <opencv2/opencv.hpp>
#include <omp.h>
#include <chrono>
// #include <tbb/parallel_for.h>
#include <utility>

std::string modelPath = "yolov8n.onnx";
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

    YOLOModel model = YOLOModel(modelPath, classNamesPath);
    YOLODetector detector = YOLODetector(model);
    cv::VideoCapture cap2(videoPath);
    cv::Mat frame;

#ifdef _OPENMP
    double start_time = omp_get_wtime();
#else
    auto start_time = std::chrono::high_resolution_clock::now();
#endif

#pragma omp parallel
    {
        cv::VideoCapture local_cap(videoPath);
        cv::Mat local_frame;
        std::vector<cvision::Frame> local_frames;

#pragma omp for 
        for (i = 0; i < totalFrames; ++i)
        {
            // Read current frame
            local_cap.set(cv::CAP_PROP_POS_FRAMES, i);
            local_cap.read(local_frame);

            // Get detections for the current frame
            // Critical section needed because cv::dnn::Net is not thread-safe
            std::vector<Detection> current_detections;
#pragma omp critical
            {
                current_detections = detector.detectObjects(local_frame);
            }

            Frame f;
            f.frameNumber = i;
            f.ts = static_cast<int>(i / fps);
            f.detections = current_detections;
            f.tsStr = cv::format("%02d:%02d:%02d", f.ts / 3600, (f.ts % 3600) / 60, f.ts % 60);

            local_frames.push_back(f);
        }

        // Copy local results to global frames array
#pragma omp critical
        {
            for (const auto& f : local_frames)
            {
                frames[f.frameNumber] = f;
            }
        }
    }

#ifdef _OPENMP
    double end_time = omp_get_wtime();
    std::cout << "Execution time: " << end_time - start_time << " seconds" << std::endl;
#else
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;
    std::cout << "Execution time: " << elapsed.count() << " seconds" << std::endl;
#endif

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