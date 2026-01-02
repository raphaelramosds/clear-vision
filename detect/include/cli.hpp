#include <iostream>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

struct CliOptions
{
    std::string videoPath;
    std::string modelPath;
    std::string configPath;
    std::string classNamesPath;
    std::string outputPath;
    std::string jsonOutputPath;
    int frameSkip = 1;
    float confidenceThreshold = 0.5f;
};

void printUsage(const std::string &programName)
{
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

bool readArgs(int argc, char *argv[], CliOptions &options)
{
    if (argc < 2)
    {
        return false;
    }

    options.videoPath = argv[1];

    for (int i = 2; i < argc; ++i)
    {
        std::string arg = argv[i];

        if ((arg == "-m" || arg == "--model") && i + 1 < argc)
        {
            options.modelPath = argv[++i];
        }
        else if ((arg == "-c" || arg == "--config") && i + 1 < argc)
        {
            options.configPath = argv[++i];
        }
        else if ((arg == "-n" || arg == "--names") && i + 1 < argc)
        {
            options.classNamesPath = argv[++i];
        }
        else if ((arg == "-o" || arg == "--output") && i + 1 < argc)
        {
            options.outputPath = argv[++i];
        }
        else if ((arg == "-j" || arg == "--json") && i + 1 < argc)
        {
            options.jsonOutputPath = argv[++i];
        }
        else if ((arg == "-s" || arg == "--skip") && i + 1 < argc)
        {
            options.frameSkip = std::stoi(argv[++i]);
        }
        else if ((arg == "-t" || arg == "--threshold") && i + 1 < argc)
        {
            options.confidenceThreshold = std::stof(argv[++i]);
        }
        else if (arg == "-h" || arg == "--help")
        {
            return false;
        }
    }

    return true;
}

bool validateOptions(const CliOptions &options)
{
    if (!fs::exists(options.videoPath))
    {
        std::cerr << "Video file not found: " << options.videoPath << std::endl;
        return false;
    }

    if (options.modelPath.empty())
    {
        std::cerr << "Error: Model path is required (-m or --model)" << std::endl;
        return false;
    }

    if (!fs::exists(options.modelPath))
    {
        std::cerr << "Model file not found: " << options.modelPath << std::endl;
        return false;
    }

    return true;
}

inline void cliHandler(int argc, char *argv[], CliOptions &options)
{
    if (!readArgs(argc, argv, options)) {
        printUsage(argv[0]);
        exit(1);
    }

    if (!validateOptions(options)) {
        exit(1);
    }

    std::cout << "Arguments parsed successfully." << std::endl;
}