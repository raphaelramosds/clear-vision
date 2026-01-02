#include "../include/video-processor.hpp"
#include "../include/levenshtein-search.hpp"
#include "../include/cli.hpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

int main(int argc, char *argv[])
{
    CliOptions options;
    cliHandler(argc, argv, options);

    // Create video processor
    VideoProcessor processor(options.videoPath, options.outputPath);
    processor.setConfidenceThreshold(options.confidenceThreshold);
    processor.setFrameSkip(options.frameSkip);

    // Load model
    if (!processor.loadModel(options.modelPath, options.configPath, options.classNamesPath)) {
        std::cerr << "Failed to load YOLO model" << std::endl;
        return 1;
    }

    // Process video
    if (!options.jsonOutputPath.empty()) {
        if (!processor.processVideoToJSON(options.jsonOutputPath)) {
            return 1;
        }

        // Query a object and save results
        LevenshteinSearch w2vSearch("output.json");
        std::vector<SearchResult> results = w2vSearch.search("a truck", 0.3f);
        w2vSearch.toJson("search_results.json", results);
    }
    else {
        if (!processor.processVideo()) {
            return 1;
        }
    }

    return 0;
}
