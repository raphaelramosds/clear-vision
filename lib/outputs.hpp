#pragma once

#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <sys/stat.h>

inline std::string create_output_directory(const std::string& base_dir, const std::string& object_name)
{
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    std::tm* tm_now = std::localtime(&time_t_now);
    
    std::ostringstream oss;
    oss << base_dir << "/"
        << std::setfill('0') 
        << std::setw(4) << (tm_now->tm_year + 1900)
        << std::setw(2) << (tm_now->tm_mon + 1)
        << std::setw(2) << tm_now->tm_mday
        << "-"
        << std::setw(2) << tm_now->tm_hour
        << std::setw(2) << tm_now->tm_min
        << std::setw(2) << tm_now->tm_sec
        << "-" << object_name;
    
    std::string dir_path = oss.str();
    
    mkdir(base_dir.c_str(), 0755);
    int result = mkdir(dir_path.c_str(), 0755);
    
    if (result != 0 && errno != EEXIST)
    {
        throw std::runtime_error("Não foi possível criar diretório: " + dir_path);
    }
    
    std::cout << "Diretório de saída criado: " << dir_path << std::endl;
    
    return dir_path;
}

inline void save_frame_as_jpeg(const cv::Mat& frame, int frame_number, const std::string& output_dir = "output")
{
    std::string filename = output_dir + "/frame_" + std::to_string(frame_number) + ".jpg";
    
    std::vector<int> compression_params;
    compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
    compression_params.push_back(95);
    
    bool success = cv::imwrite(filename, frame, compression_params);
    
    if (!success)
    {
        std::cerr << "Erro ao salvar frame " << frame_number << std::endl;
    }
}

