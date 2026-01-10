#include <iostream>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

struct cli_options
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

void print_usage(const std::string &program)
{
    std::cout << "Usage: " << program << " <video_path> [options]\n\n"
              << "Options:\n"
              << "  -m, --model <path>           Path to YOLO model (.onnx)\n"
              << "  -n, --names <path>           Path to class names file\n"
              << "  -t, --threshold <0-1>        Confidence threshold (default: 0.5)\n"
              << "  -s, --nms <0-1>              NMS threshold (default: 0.4)\n"
              << "  -h, --help                   Show this help message\n"
              << "\nExample:\n"
              << "  " << program << " video.mp4 -m yolov8n.onnx -n coco.names\n";
}

inline void cli_parse_args(int argc, char *argv[], cli_options &options)
{
    // Read args
    if (argc < 2)
    {
        print_usage(argv[0]);
        exit(1);
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
            print_usage(argv[0]);
            exit(0);
        }
    }

    // Validate options
    if (!fs::exists(options.videoPath))
    {
        std::cerr << "Video file not found: " << options.videoPath << std::endl;
        exit(1);
    }

    if (options.modelPath.empty())
    {
        std::cerr << "Error: Model path is required (-m or --model)" << std::endl;
        exit(1);
    }

    if (!fs::exists(options.modelPath))
    {
        std::cerr << "Model file not found: " << options.modelPath << std::endl;
        exit(1);
    }

    std::cout << "Arguments parsed successfully." << std::endl;
}