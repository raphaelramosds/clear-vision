#define BATCH_SIZE 128

#include <iostream>
#include <opencv2/opencv.hpp>
#include "utils.h"

int main(int argv, char **argc)
{
    const char *videoPath = "video_rua01.mp4";
    const char *onnxPath = "yolov8n.onnx";

    // Carregar rede
    cv::dnn::Net net;
    TIMEIT("loadNet: ", loadNet, onnxPath, net);

    // Amostrar N frames
    // std::vector<cv::Mat> frames = {};
    // int N = 12;
    // sampleFrames(videoPath, N, frames);
    // std::vector<cv::Mat> batch(frames.begin(), frames.begin() + 4);
    // printf("Bach possui %d frames\n", batch.size());

    // Inserir e processar um batch na rede neural
    std::vector<Detection> detections = {};
    // TIMEIT("processBatch: ", processBatch, batch, net, detections);

    // printf("%ld classes detectadas\n", detections.size());

    // Amostrar muitos frames (estouro de stack)
    // frames.clear();
    // TIMEIT("sampleFrames: ", sampleFrames, videoPath, 1000, frames);

    cv::VideoCapture cap(videoPath);
    if (!cap.isOpened())
    {
        throw std::invalid_argument(std::string("Video file does not exist: ") + videoPath);
    }

    unsigned int totalFrames = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_COUNT));
    unsigned int remainingBatches = totalFrames % BATCH_SIZE;
    unsigned int numBatches = totalFrames / BATCH_SIZE;
    unsigned int currFrame = 0;
    unsigned int currBatch = 0;
    

    // 1. Extrair primeiros BATCH_SIZE frames 
    // 2. Coloca-los em um std::vector<cv::Mat> batch
    // 3. Chamar a função processBatch(std::vector<cv::Mat> &batch, cv::dnn::Net &net, int currFrame, std::vector<Detection> &detections)
    // 4. Caso detections.size() == 0 prossiga para a leitura do próximo batch
    //    Caso contrario, salve as deteccoes e pergunte se o usuário deseja proseguir para o próximo batch

    return 0;
}