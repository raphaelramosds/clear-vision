#include "cvision.hpp"
#include "cli.hpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>

int main(int argc, char *argv[])
{
    cli_options options;
    cli_parse_args(argc, argv, options);

    // Call to API
    std::string q = "a person";
    std::vector<cvision::Frame> results = cvision::query(options.videoPath, q);

    // Get a frame and annotate it
    cvision::Frame& result = results[10];
    cv::Mat frame;
    cvision::annotate(options.videoPath, result, frame);
    cv::imshow("Result Frame", frame);
    cv::waitKey(0);

    return 0;
}
