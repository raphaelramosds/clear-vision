#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <opencv2/opencv.hpp>
#include <functional>

// Estrutura para representar uma detecção
struct Detection
{
    int classId;
    int frameNumber;
    float x, y, w, h;
    float confidence;
};

// Estrutura para configuração do processador
struct BatchProcessorConfig
{
    std::string videoPath;
    std::string onnxPath;
    std::string classNamesPath;
    std::string outputBaseDir;
    int batchSize;
    int miniBatchSize;
};

// Estrutura para resultado de processamento de um batch
struct BatchProcessingResult
{
    std::vector<Detection> allDetections;
    std::vector<Detection> targetDetections;
    int framesProcessed;
    int startFrame;
    bool success;
    std::string errorMessage;
};

// Criar diretório com timestamp e nome do objeto
inline std::string createOutputDirectory(const std::string& baseDir, const std::string& objectName)
{
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    std::tm* tm_now = std::localtime(&time_t_now);
    
    std::ostringstream oss;
    oss << baseDir << "/"
        << std::setfill('0') 
        << std::setw(4) << (tm_now->tm_year + 1900)
        << std::setw(2) << (tm_now->tm_mon + 1)
        << std::setw(2) << tm_now->tm_mday
        << "-"
        << std::setw(2) << tm_now->tm_hour
        << std::setw(2) << tm_now->tm_min
        << std::setw(2) << tm_now->tm_sec
        << "-" << objectName;
    
    std::string dirPath = oss.str();
    
    mkdir(baseDir.c_str(), 0755);
    int result = mkdir(dirPath.c_str(), 0755);
    
    if (result != 0 && errno != EEXIST)
    {
        throw std::runtime_error("Não foi possível criar diretório: " + dirPath);
    }
    
    std::cout << "Diretório de saída criado: " << dirPath << std::endl;
    
    return dirPath;
}

// Carregar nomes das classes COCO de um arquivo
inline std::vector<std::string> loadClassNames(const char* filepath)
{
    std::vector<std::string> classNames;
    std::ifstream file(filepath);
    
    if (!file.is_open())
    {
        throw std::runtime_error(std::string("Cannot open class names file: ") + filepath);
    }
    
    std::string line;
    while (std::getline(file, line))
    {
        if (!line.empty())
        {
            classNames.push_back(line);
        }
    }
    
    file.close();
    return classNames;
}

// Busca simples por substring (case insensitive)
inline int findClassId(const std::string& query, const std::vector<std::string>& classNames)
{
    std::string queryLower = query;
    std::transform(queryLower.begin(), queryLower.end(), queryLower.begin(), ::tolower);
    
    for (size_t i = 0; i < classNames.size(); ++i)
    {
        std::string className = classNames[i];
        std::transform(className.begin(), className.end(), className.begin(), ::tolower);
        
        if (className.find(queryLower) != std::string::npos)
        {
            return static_cast<int>(i);
        }
    }
    
    return -1;
}

// Filtrar detecções por classId
inline std::vector<Detection> filterDetectionsByClass(const std::vector<Detection>& detections, int classId)
{
    std::vector<Detection> filtered;
    
    for (const auto& det : detections)
    {
        if (det.classId == classId)
        {
            filtered.push_back(det);
        }
    }
    
    return filtered;
}

// Desenhar detecções no frame
inline cv::Mat drawDetections(const cv::Mat& frame, const std::vector<Detection>& detections, 
                              const std::vector<std::string>& classNames)
{
    cv::Mat annotatedFrame = frame.clone();
    
    for (const auto& det : detections)
    {
        cv::Rect bbox(static_cast<int>(det.x), static_cast<int>(det.y), 
                      static_cast<int>(det.w), static_cast<int>(det.h));
        cv::rectangle(annotatedFrame, bbox, cv::Scalar(0, 255, 0), 2);
        
        std::string label = classNames[det.classId] + ": " + 
                           std::to_string(static_cast<int>(det.confidence * 100)) + "%";
        
        int baseline = 0;
        cv::Size textSize = cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX, 0.6, 2, &baseline);
        
        cv::Point textOrg(det.x, det.y - 5);
        if (textOrg.y - textSize.height < 0)
        {
            textOrg.y = det.y + textSize.height + 5;
        }
        
        cv::rectangle(annotatedFrame, 
                     cv::Point(textOrg.x, textOrg.y - textSize.height - baseline),
                     cv::Point(textOrg.x + textSize.width, textOrg.y + baseline),
                     cv::Scalar(0, 255, 0), cv::FILLED);
        
        cv::putText(annotatedFrame, label, textOrg, cv::FONT_HERSHEY_SIMPLEX, 
                   0.6, cv::Scalar(0, 0, 0), 2);
    }
    
    return annotatedFrame;
}

// Salvar frame como JPEG
inline void saveFrameAsJpeg(const cv::Mat& frame, int frameNumber, const std::string& outputDir = "output")
{
    std::string filename = outputDir + "/frame_" + std::to_string(frameNumber) + ".jpg";
    
    std::vector<int> compression_params;
    compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
    compression_params.push_back(95);
    
    bool success = cv::imwrite(filename, frame, compression_params);
    
    if (!success)
    {
        std::cerr << "Erro ao salvar frame " << frameNumber << std::endl;
    }
}

// Carregar rede neural
inline void loadNet(const char* onnxPath, cv::dnn::Net& net)
{
    net = cv::dnn::readNetFromONNX(onnxPath);
    net.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
    net.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA_FP16);

    if (net.empty())
    {
        throw std::runtime_error("Model not loaded");
    }
}

// Processar batch de frames através da rede neural
inline void processBatch(std::vector<cv::Mat>& batch, cv::dnn::Net& net, int startFrame, 
                         std::vector<Detection>& detections)
{
    float confidenceThreshold = 0.5;
    float nmsThreshold = 0.4;
    int inputWidth = 640;
    int inputHeight = 640;
    float scale = 1.0 / 255.0;
    cv::Scalar mean = cv::Scalar(0, 0, 0);
    bool swapRB = true;
    bool crop = false;

    cv::Mat blob = cv::dnn::blobFromImages(batch, scale, cv::Size(inputWidth, inputHeight),
                                           mean, swapRB, crop);
    net.setInput(blob);

    std::vector<cv::Mat> outputs;
    net.forward(outputs, net.getUnconnectedOutLayersNames());

    cv::Mat output = outputs.front();

    int batchSize = output.size[0];
    int numAttributes = output.size[1];
    int numPredictions = output.size[2];

    for (int b = 0; b < batchSize; ++b)
    {
        std::vector<int> classIds;
        std::vector<float> confidences;
        std::vector<cv::Rect> boxes;

        float xScale = batch[0].cols / static_cast<float>(inputWidth);
        float yScale = batch[0].rows / static_cast<float>(inputHeight);
        float x, y, w, h;

        float* batchData = output.ptr<float>() + (b * numAttributes * numPredictions);
        cv::Mat row = cv::Mat(numAttributes, numPredictions, CV_32F, batchData);
        cv::Mat outputReshaped = row.t();

        for (int i = 0; i < numPredictions; ++i)
        {
            const float* ptr = outputReshaped.ptr<float>(i);
            int classId = 0;
            float maxProb = 0.0f;

            for (int c = 4; c < numAttributes; ++c)
            {
                float prob = ptr[c];
                if (prob > maxProb)
                {
                    maxProb = prob;
                    classId = c - 4;
                }
            }

            if (maxProb >= confidenceThreshold)
            {
                x = ptr[0] * xScale;
                y = ptr[1] * yScale;
                w = ptr[2] * xScale;
                h = ptr[3] * yScale;

                boxes.push_back(cv::Rect(
                    static_cast<int>(x - w / 2),
                    static_cast<int>(y - h / 2),
                    static_cast<int>(w),
                    static_cast<int>(h)));
                confidences.push_back(maxProb);
                classIds.push_back(classId);
            }
        }

        std::vector<int> indices;
        cv::dnn::NMSBoxes(boxes, confidences, confidenceThreshold, nmsThreshold, indices);

        for (int idx : indices)
        {
            Detection det;
            det.classId = classIds[idx];
            det.frameNumber = startFrame + b;
            det.x = boxes[idx].x;
            det.y = boxes[idx].y;
            det.w = boxes[idx].width;
            det.h = boxes[idx].height;
            det.confidence = confidences[idx];

            detections.emplace_back(det);
        }
    }
}

// Inicializar modelo neural e classes
inline bool initModelAndClasses(const std::string& onnxPath, 
                                const std::string& classNamesPath,
                                cv::dnn::Net& net,
                                std::vector<std::string>& classNames,
                                std::string& errorMsg)
{
    try
    {
        classNames = loadClassNames(classNamesPath.c_str());
        loadNet(onnxPath.c_str(), net);
        return true;
    }
    catch (const std::exception& e)
    {
        errorMsg = e.what();
        return false;
    }
}

// Ler próximo batch de frames do vídeo
inline bool readNextBatch(cv::VideoCapture& cap, 
                          int batchSize,
                          std::vector<cv::Mat>& batch,
                          unsigned int& currentFrame,
                          unsigned int totalFrames)
{
    batch.clear();
    
    for (int i = 0; i < batchSize && currentFrame < totalFrames; ++i, ++currentFrame)
    {
        cv::Mat frame;
        bool success = cap.read(frame);
        
        if (!success || frame.empty())
        {
            return false;
        }
        
        batch.push_back(frame.clone());
    }
    
    return !batch.empty();
}

// Processar batch de frames com detecção de objetos
inline BatchProcessingResult processBatchDetections(std::vector<cv::Mat>& batch,
                                                    cv::dnn::Net& net,
                                                    int startFrame,
                                                    int targetClassId,
                                                    int miniBatchSize = 4)
{
    BatchProcessingResult result;
    result.startFrame = startFrame;
    result.framesProcessed = batch.size();
    result.success = true;
    
    try
    {
        for (size_t i = 0; i < batch.size(); i += miniBatchSize)
        {
            size_t end = std::min(i + miniBatchSize, batch.size());
            std::vector<cv::Mat> miniBatch(batch.begin() + i, batch.begin() + end);
            int miniStartFrame = startFrame + i;
            int originalSize = miniBatch.size();
            
            while (miniBatch.size() < miniBatchSize)
            {
                cv::Mat paddingFrame = cv::Mat::zeros(miniBatch[0].rows, miniBatch[0].cols, 
                                                      miniBatch[0].type());
                miniBatch.push_back(paddingFrame);
            }
            
            size_t detectionsBefore = result.allDetections.size();
            processBatch(miniBatch, net, miniStartFrame, result.allDetections);
            
            if (originalSize < miniBatchSize)
            {
                std::vector<Detection> validDetections;
                for (size_t d = detectionsBefore; d < result.allDetections.size(); ++d)
                {
                    if (result.allDetections[d].frameNumber < miniStartFrame + originalSize)
                    {
                        validDetections.push_back(result.allDetections[d]);
                    }
                }
                result.allDetections.erase(result.allDetections.begin() + detectionsBefore, 
                                          result.allDetections.end());
                result.allDetections.insert(result.allDetections.end(), 
                                           validDetections.begin(), validDetections.end());
            }
        }
        
        result.targetDetections = filterDetectionsByClass(result.allDetections, targetClassId);
    }
    catch (const std::exception& e)
    {
        result.success = false;
        result.errorMessage = e.what();
    }
    
    return result;
}

// Salvar frames com detecções
inline int saveDetectedFrames(const std::vector<cv::Mat>& batch,
                              const std::vector<Detection>& targetDetections,
                              const std::vector<std::string>& classNames,
                              int startFrame,
                              const std::string& outputDir)
{
    std::vector<int> uniqueFrames;
    for (const auto& det : targetDetections)
    {
        if (std::find(uniqueFrames.begin(), uniqueFrames.end(), det.frameNumber) == uniqueFrames.end())
        {
            uniqueFrames.push_back(det.frameNumber);
        }
    }
    
    for (int frameNum : uniqueFrames)
    {
        int batchIndex = frameNum - startFrame;
        if (batchIndex >= 0 && batchIndex < batch.size())
        {
            std::vector<Detection> frameDetections;
            for (const auto& det : targetDetections)
            {
                if (det.frameNumber == frameNum)
                {
                    frameDetections.push_back(det);
                }
            }
            
            cv::Mat annotatedFrame = drawDetections(batch[batchIndex], frameDetections, classNames);
            saveFrameAsJpeg(annotatedFrame, frameNum, outputDir);
        }
    }
    
    return uniqueFrames.size();
}

// Processar vídeo completo (função principal)
inline bool processVideo(const BatchProcessorConfig& config,
                         int targetClassId,
                         const std::vector<std::string>& classNames,
                         cv::dnn::Net& net,
                         const std::string& outputDir,
                         std::function<bool(int, int, int, int, int, unsigned int)> onBatchProcessed = nullptr)
{
    cv::VideoCapture cap(config.videoPath);
    if (!cap.isOpened())
    {
        return false;
    }
    
    unsigned int totalFrames = static_cast<unsigned int>(cap.get(cv::CAP_PROP_FRAME_COUNT));
    unsigned int currentFrame = 0;
    unsigned int batchCount = 0;
    
    while (currentFrame < totalFrames)
    {
        batchCount++;
        
        std::vector<cv::Mat> batch;
        if (!readNextBatch(cap, config.batchSize, batch, currentFrame, totalFrames))
        {
            break;
        }
        
        int startFrame = currentFrame - batch.size();
        int endFrame = currentFrame - 1;
        
        BatchProcessingResult result = processBatchDetections(batch, net, startFrame, 
                                                              targetClassId, config.miniBatchSize);
        
        if (!result.success)
        {
            cap.release();
            return false;
        }
        
        int savedFrames = 0;
        if (!result.targetDetections.empty())
        {
            savedFrames = saveDetectedFrames(batch, result.targetDetections, classNames, 
                                            startFrame, outputDir);
        }
        
        if (onBatchProcessed)
        {
            bool shouldContinue = onBatchProcessed(batchCount, result.targetDetections.size(), 
                                                  savedFrames, startFrame, endFrame, totalFrames);
            if (!shouldContinue)
            {
                break;
            }
        }
    }
    
    cap.release();
    return true;
}

#endif // PROCESSOR_H
