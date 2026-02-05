#include <iostream>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string.h>
#include <vector>

struct Config
{
    // Batch
    uint16_t batch_size;
    uint8_t batch_mini_size;
    // Net
    const char *onnx_path;
    uint16_t input_width;
    uint16_t input_height;
    float nms_thr;
    float thr;
    // App
    const char *output_dir;
};

void split_property(const std::string &line, std::string &key, std::string &value)
{
    char str = '=';
    std::string token;
    std::istringstream token_stream(line);

    // Fill key
    std::getline(token_stream, token, '=');
    key = token;

    // Fill value
    std::getline(token_stream, token);
    value = token;
}

void parse_conf(Config &config)
{
    std::cout << "Lendo arquivo de configuracao\n";

    std::ifstream fin;
    fin.open("cvision.conf");

    if (!fin.is_open())
    {
        std::cerr << "Nao foi possivel abrir o arquivo de configuracao\n";
        exit(1);
    }

    std::string line, key, value;

    while (std::getline(fin, line))
    {
        std::stringstream iss(line);
        if (!(iss >> line))
            continue; // Skip empty lines

        if (line == "[batch]")
        {
            for (int i = 0; i < 2; i++)
            {
                std::getline(fin, line);
                split_property(line, key, value);
                if (key == "size")
                {
                    config.batch_size = std::stoi(value);
                }
                else if (key == "mini_size")
                {
                    config.batch_mini_size = std::stoi(value);
                }
                else {
                    std::cerr << "Propriedade desconhecida: " << key << std::endl;
                }
            }
       }

       else if (line == "[net]")
       {
            for (int i = 0; i < 5; i++)
            {
                std::getline(fin, line);
                split_property(line, key, value);
                if (key == "onnx_path")
                {
                    config.onnx_path = strdup(value.c_str());
                }
                else if (key == "input_width")
                {
                    config.input_width = std::stoi(value);
                }
                else if (key == "input_height")
                {
                    config.input_height = std::stoi(value);
                }
                else if (key == "nms_thr")
                {
                    config.nms_thr = std::stof(value);
                }
                else if (key == "thr")
                {
                    config.thr = std::stof(value);
                }
                else {
                    std::cerr << "Propriedade desconhecida: " << key << std::endl;
                }
            }
       }

       else if (line == "[app]")
       {
            for (int i = 0; i < 1; i++)
            {
                std::getline(fin, line);
                split_property(line, key, value);
                if (key == "output_dir")
                {
                    config.output_dir = strdup(value.c_str());
                }
                else {
                    std::cerr << "Propriedade desconhecida: " << key << std::endl;
                }
            }
       }

       else
       {
           std::cerr << "Secao desconhecida: " << line << std::endl;
       }
    }

    std::cout << "Arquivo de configuracao aberto com sucesso\n";
}

int main()
{
    Config c;
    parse_conf(c);

    // Check parsed values
    std::cout << "Batch size: " << c.batch_size << std::endl;
    std::cout << "Batch mini size: " << (int)c.batch_mini_size << std::endl;
    std::cout << "ONNX path: " << c.onnx_path << std::endl;
    std::cout << "Input width: " << c.input_width << std::endl;
    std::cout << "Input height: " << c.input_height << std::endl;
    std::cout << "NMS threshold: " << c.nms_thr << std::endl;
    std::cout << "Threshold: " << c.thr << std::endl;
    std::cout << "Output directory: " << c.output_dir << std::endl;

    return 0;
}