#include "../include/video-processor.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

void printUsage(const std::string& programName) {
    std::cout << "Usage: " << programName << " <video_path> [options]\n\n"
             << "Options:\n"
             << "  -m, --model <path>           Path to YOLO model (.onnx)\n"
             << "  -c, --config <path>          Path to YOLO config (.cfg) for .weights models\n"
             << "  -n, --names <path>           Path to class names file\n"
             << "  -o, --output <path>          Output video path (mp4)\n"
             << "  -j, --json <path>            Save detections to JSON file\n"
             << "  -s, --skip <n>               Process every Nth frame (default: 1)\n"
             << "  -t, --threshold <0-1>        Confidence threshold (default: 0.5)\n"
             << "  -h, --help                   Show this help message\n"
             << "\nExample:\n"
             << "  " << programName << " video.mp4 -m yolov8n.onnx -n coco.names -o output.mp4\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    std::string videoPath = argv[1];
    std::string modelPath;
    std::string configPath;
    std::string classNamesPath;
    std::string outputPath;
    std::string jsonOutputPath;
    int frameSkip = 1;
    float confidenceThreshold = 0.5f;

    // Parse command line arguments
    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];

        if ((arg == "-m" || arg == "--model") && i + 1 < argc) {
            modelPath = argv[++i];
        } else if ((arg == "-c" || arg == "--config") && i + 1 < argc) {
            configPath = argv[++i];
        } else if ((arg == "-n" || arg == "--names") && i + 1 < argc) {
            classNamesPath = argv[++i];
        } else if ((arg == "-o" || arg == "--output") && i + 1 < argc) {
            outputPath = argv[++i];
        } else if ((arg == "-j" || arg == "--json") && i + 1 < argc) {
            jsonOutputPath = argv[++i];
        } else if ((arg == "-s" || arg == "--skip") && i + 1 < argc) {
            frameSkip = std::stoi(argv[++i]);
        } else if ((arg == "-t" || arg == "--threshold") && i + 1 < argc) {
            confidenceThreshold = std::stof(argv[++i]);
        } else if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return 0;
        }
    }

    // Validate inputs
    if (!fs::exists(videoPath)) {
        std::cerr << "Video file not found: " << videoPath << std::endl;
        return 1;
    }

    if (modelPath.empty()) {
        std::cerr << "Error: Model path is required (-m or --model)" << std::endl;
        return 1;
    }

    if (!fs::exists(modelPath)) {
        std::cerr << "Model file not found: " << modelPath << std::endl;
        return 1;
    }

    // Create video processor
    VideoProcessor processor(videoPath, outputPath);
    processor.setConfidenceThreshold(confidenceThreshold);
    processor.setFrameSkip(frameSkip);

    // Load model
    if (!processor.loadModel(modelPath, configPath, classNamesPath)) {
        std::cerr << "Failed to load YOLO model" << std::endl;
        return 1;
    }

    // Process video
    if (!jsonOutputPath.empty()) {
        if (!processor.processVideoToJSON(jsonOutputPath)) {
            return 1;
        }
    } else {
        if (!processor.processVideo()) {
            return 1;
        }
    }

    return 0;
}
