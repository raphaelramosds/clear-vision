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
    std::string q = "person";
    std::vector<cvision::Frame> results = cvision::query(options.videoPath, q);

    // Draw detections on the frame
    cvision::Frame result = results[0];
    cv::Mat frame;
    cv::VideoCapture cap(options.videoPath);
    cap.set(cv::CAP_PROP_POS_FRAMES, result.frameNumber);
    cap.read(frame);
    
    // detector.drawDetections(frame, result.detections);
    cv::imshow("Result Frame", frame);
    cap.release();

    cv::waitKey(0);

    return 0;
}
