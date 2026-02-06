#include <opencv2/opencv.hpp>

#include "cvision.hpp"
#include "conf.hpp"

// Globais (estados internos)
Config config;
cv::dnn::Net net;
std::vector<std::string> class_names;

void cvision::bootstrap(const std::string conf_path)
{
    // Carrega parametros conforme arquivo de configuracao
    parse_conf(config);

    // Inicializa rede neural
    net = cv::dnn::readNetFromONNX(config.onnx_path);
    if (net.empty())
    {
        std::cerr << "Erro ao carregar o modelo ONNX: " << config.onnx_path << std::endl;
        std::exit(EXIT_FAILURE);
    }

    // Inicializa nomes das classes
    std::ifstream class_names_file(config.classnames_path);
    std::string className;
    while (std::getline(class_names_file, className))
    {
        class_names.push_back(className);
    }
    class_names_file.close();
    std::cout << "Carregadas " << class_names.size() << " classes" << std::endl;

    std::cout << "Inicializacao concluida com sucesso\n";
}

