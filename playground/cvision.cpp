#include <opencv2/opencv.hpp>
#include <string>

#include "cvision.hpp"
#include "conf.hpp"

cv::dnn::Net net;

void cvision::bootstrap(const std::string conf_path)
{
    Config config;
    parse_conf(config);

    net = cv::dnn::readNetFromONNX(config.onnx_path);
    if (net.empty())
    {
        std::cerr << "Erro ao carregar o modelo ONNX: " << config.onnx_path << std::endl;
        std::exit(EXIT_FAILURE);
    }
}