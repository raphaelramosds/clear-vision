#include "../include/video-processor.hpp"

#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <chrono>

VideoProcessor::VideoProcessor(const std::string& video, const std::string& output)
    : videoPath(video), outputPath(output), saveVideo(!output.empty()), frameSkip(1) {}

bool VideoProcessor::loadModel(const std::string& modelPath,
                                const std::string& configPath,
                                const std::string& classNamesPath) {
    return detector.loadModel(modelPath, configPath, classNamesPath);
}

bool VideoProcessor::processVideo() {
    cv::VideoCapture cap(videoPath);

    if (!cap.isOpened()) {
        std::cerr << "Cannot open video: " << videoPath << std::endl;
        return false;
    }

    // Get video properties
    int frameCount = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_COUNT));
    double fps = cap.get(cv::CAP_PROP_FPS);
    int width = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
    int height = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));

    std::cout << "\nVideo Properties:" << std::endl;
    std::cout << "  Frames: " << frameCount << std::endl;
    std::cout << "  FPS: " << fps << std::endl;
    std::cout << "  Resolution: " << width << "x" << height << std::endl;

    // Setup video writer if output requested
    cv::VideoWriter writer;
    if (saveVideo) {
        int fourcc = cv::VideoWriter::fourcc('m', 'p', '4', 'v');
        writer.open(outputPath, fourcc, fps, cv::Size(width, height));
        if (!writer.isOpened()) {
            std::cerr << "Cannot create output video: " << outputPath << std::endl;
            return false;
        }
    }

    cv::Mat frame;
    int processedFrames = 0;
    int totalFrames = 0;
    auto startTime = std::chrono::high_resolution_clock::now();

    while (true) {
        if (!cap.read(frame)) {
            break;
        }

        totalFrames++;

        // Process every Nth frame
        if (totalFrames % frameSkip == 0) {
            // Detect objects
            auto detections = detector.detectObjects(frame);

            std::cout << "\rFrame " << totalFrames << "/" << frameCount 
                     << " - Detections: " << detections.size() << std::flush;

            // Draw detections on frame
            detector.drawDetections(frame, detections);
            processedFrames++;
        }

        // Write frame to output
        if (saveVideo) {
            writer.write(frame);
        }
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(
        endTime - startTime
    );

    std::cout << "\n\nProcessing complete!" << std::endl;
    std::cout << "Total frames: " << totalFrames << std::endl;
    std::cout << "Processed frames: " << processedFrames << std::endl;
    std::cout << "Processing time: " << duration.count() << " seconds" << std::endl;
    std::cout << "FPS processed: " << (processedFrames / static_cast<float>(duration.count())) 
             << std::endl;

    if (saveVideo) {
        std::cout << "Output saved to: " << outputPath << std::endl;
    }

    cap.release();
    if (writer.isOpened()) {
        writer.release();
    }

    return true;
}

bool VideoProcessor::processVideoToJSON(const std::string& jsonOutputPath) {
    cv::VideoCapture cap(videoPath);

    if (!cap.isOpened()) {
        std::cerr << "Cannot open video: " << videoPath << std::endl;
        return false;
    }

    int frameCount = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_COUNT));
    int totalFrames = 0;
    int step = cap.get(cv::CAP_PROP_FPS) * 1;

    std::ofstream jsonFile(jsonOutputPath);
    jsonFile << "[\n";

    cv::Mat frame;
    bool firstFrame = true;

    while (cap.read(frame)) {
        totalFrames++;
        
        if (totalFrames % step == 0) {

            auto detections = detector.detectObjects(frame);
    
            if (!firstFrame) {
                jsonFile << ",\n";
            }
            firstFrame = false;

            jsonFile << "  {\n";
            jsonFile << "    \"frame_number\": " << totalFrames << ",\n";
            jsonFile << "    \"detections\": [\n";

            for (size_t i = 0; i < detections.size(); ++i) {
                const auto& det = detections[i];
                u_long ts = static_cast<u_long>(totalFrames) / step;

                int hours = ts / 3600;
                int minutes = (ts % 3600) / 60;
                int seconds = ts % 60;
                
                char timeBuffer[9];
                std::snprintf(timeBuffer, sizeof(timeBuffer), "%02d:%02d:%02d", hours, minutes, seconds);

                jsonFile << "      {\n";
                jsonFile << "        \"ts\": " << ts << ",\n";
                jsonFile << "        \"tsStr\": \"" << timeBuffer << "\",\n";
                jsonFile << "        \"class_name\": \"" << det.className << "\",\n";
                jsonFile << "        \"confidence\": " << det.confidence << ",\n";
                jsonFile << "        \"x\": " << det.x << ",\n";
                jsonFile << "        \"y\": " << det.y << ",\n";
                jsonFile << "        \"width\": " << det.w << ",\n";
                jsonFile << "        \"height\": " << det.h << "\n";
                jsonFile << "      }";
                if (i < detections.size() - 1) jsonFile << ",";
                jsonFile << "\n";
            }
    
            jsonFile << "    ]\n";
            jsonFile << "  }";

            std::cout << "\rProcessed " << totalFrames << "/" << frameCount 
                     << " frames..." << std::flush;
        }
    }
    jsonFile << "\n]\n";
    jsonFile.close();

    std::cout << "\nJSON output saved to: " << jsonOutputPath << std::endl;
    cap.release();

    return true;
}

void VideoProcessor::setFrameSkip(int skip) {
    frameSkip = skip;
}

void VideoProcessor::setConfidenceThreshold(float threshold) {
    detector.setConfidenceThreshold(threshold);
}

YOLODetector& VideoProcessor::getDetector() {
    return detector;
}