#define BATCH_SIZE 128

#include <iostream>
#include <opencv2/opencv.hpp>
#include "utils.h"

int main(int argc, char* argv[])
{
    // Verificar se o caminho do vídeo foi fornecido
    if (argc < 2)
    {
        std::cerr << "Uso: " << argv[0] << " <caminho_do_video>" << std::endl;
        std::cerr << "Exemplo: " << argv[0] << " video_rua01.mp4" << std::endl;
        return 1;
    }

    const char *videoPath = argv[1];
    const char *onnxPath = "yolov8n.onnx";
    const char *classNamesPath = "../core/coco.names";
    const char *outputDir = "output";

    // Carregar nomes das classes
    std::vector<std::string> classNames;
    try
    {
        classNames = load_class_names(classNamesPath);
        std::cout << "Carregadas " << classNames.size() << " classes" << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Erro: " << e.what() << std::endl;
        return 1;
    }

    // Carregar rede neural
    cv::dnn::Net net;
    try
    {
        TIMEIT("Carregando modelo: ", loadNet, onnxPath, net);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Erro: " << e.what() << std::endl;
        return 1;
    }

    // Perguntar qual objeto o usuario deseja buscar
    std::cout << "\nDigite o nome do objeto que deseja buscar (ex: person, car, dog): ";
    std::string objectQuery;
    std::getline(std::cin, objectQuery);

    int targetClassId = find_class_id(objectQuery, classNames);
    if (targetClassId == -1)
    {
        std::cerr << "Objeto '" << objectQuery << "' nao encontrado nas classes COCO" << std::endl;
        return 1;
    }

    std::cout << "Buscando por: " << classNames[targetClassId] << " (ID " << targetClassId << ")" << std::endl;

    // Criar diretório de saída com timestamp e nome do objeto
    std::string outputDirPath;
    try
    {
        outputDirPath = create_output_directory(outputDir, classNames[targetClassId]);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Erro ao criar diretório: " << e.what() << std::endl;
        return 1;
    }

    // Abrir video
    cv::VideoCapture cap(videoPath);
    if (!cap.isOpened())
    {
        std::cerr << "Erro ao abrir video: " << videoPath << std::endl;
        return 1;
    }

    unsigned int totalFrames = static_cast<unsigned int>(cap.get(cv::CAP_PROP_FRAME_COUNT));
    unsigned int currentFrame = 0;
    unsigned int batchCount = 0;

    std::cout << "Total de frames no video: " << totalFrames << std::endl;
    std::cout << "Processando em batches de " << BATCH_SIZE << " frames" << std::endl;

    // Loop principal: processar batches
    while (currentFrame < totalFrames)
    {
        batchCount++;
        std::cout << "\n=== Batch " << batchCount << " ===" << std::endl;

        // Ler BATCH_SIZE frames
        std::vector<cv::Mat> batch;
        for (int i = 0; i < BATCH_SIZE && currentFrame < totalFrames; ++i, ++currentFrame)
        {
            cv::Mat frame;
            bool success = cap.read(frame);
            
            if (!success || frame.empty())
            {
                std::cerr << "Erro ao ler frame " << currentFrame << std::endl;
                break;
            }
            
            batch.push_back(frame.clone());
        }

        if (batch.empty())
        {
            std::cout << "Nenhum frame lido. Fim do video." << std::endl;
            break;
        }

        std::cout << "Lidos " << batch.size() << " frames (frame " 
                  << (currentFrame - batch.size()) << " a " << (currentFrame - 1) << ")" << std::endl;

        // Aplicar deteccoes em todos os frames do batch
        // Processar em mini-batches de 4 frames (limite da rede neural)
        std::vector<Detection> detections;
        int startFrame = currentFrame - batch.size();
        const int MINI_BATCH_SIZE = 4;
        
        try
        {
            for (size_t i = 0; i < batch.size(); i += MINI_BATCH_SIZE)
            {
                size_t end = std::min(i + MINI_BATCH_SIZE, batch.size());
                std::vector<cv::Mat> miniBatch(batch.begin() + i, batch.begin() + end);
                int miniStartFrame = startFrame + i;
                int originalSize = miniBatch.size();
                
                // Aplicar padding se necessario (mini-batch incompleto)
                while (miniBatch.size() < MINI_BATCH_SIZE)
                {
                    // Criar frame preto do mesmo tamanho do primeiro frame
                    cv::Mat paddingFrame = cv::Mat::zeros(miniBatch[0].rows, miniBatch[0].cols, miniBatch[0].type());
                    miniBatch.push_back(paddingFrame);
                }
                
                // Processar o mini-batch (sempre com 4 frames)
                size_t detectionsBefore = detections.size();
                processBatch(miniBatch, net, miniStartFrame, detections);
                
                // Remover deteccoes frames de padding
                if (originalSize < MINI_BATCH_SIZE)
                {
                    std::vector<Detection> validDetections;
                    for (size_t d = detectionsBefore; d < detections.size(); ++d)
                    {
                        // Manter apenas detecções dos frames originais
                        if (detections[d].frameNumber < miniStartFrame + originalSize)
                        {
                            validDetections.push_back(detections[d]);
                        }
                    }
                    // Remover todas as deteccoes deste mini-batch e adicionar apenas as validas
                    detections.erase(detections.begin() + detectionsBefore, detections.end());
                    detections.insert(detections.end(), validDetections.begin(), validDetections.end());
                }
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << "Erro ao processar batch: " << e.what() << std::endl;
            break;
        }

        std::cout << "Total de deteccoes: " << detections.size() << std::endl;

        // Filtrar deteccoes do objeto solicitado
        std::vector<Detection> targetDetections = filter_detections_by_class(detections, targetClassId);
        
        std::cout << "Deteccoes de '" << classNames[targetClassId] << "': " << targetDetections.size() << std::endl;

        // Salvar frames que contem o objeto
        if (!targetDetections.empty())
        {
            // Extrair números de frames únicos
            std::vector<int> uniqueFrames;
            for (const auto &det : targetDetections)
            {
                if (std::find(uniqueFrames.begin(), uniqueFrames.end(), det.frameNumber) == uniqueFrames.end())
                {
                    uniqueFrames.push_back(det.frameNumber);
                }
            }

            std::cout << "Salvando " << uniqueFrames.size() << " frames..." << std::endl;
            
            for (int frameNum : uniqueFrames)
            {
                int batchIndex = frameNum - startFrame;
                if (batchIndex >= 0 && batchIndex < batch.size())
                {
                    // Filtrar detecções deste frame específico
                    std::vector<Detection> frameDetections;
                    for (const auto& det : targetDetections)
                    {
                        if (det.frameNumber == frameNum)
                        {
                            frameDetections.push_back(det);
                        }
                    }
                    
                    // Desenhar detecções no frame
                    cv::Mat annotatedFrame = draw_detections(batch[batchIndex], frameDetections, classNames);
                    
                    // Salvar frame anotado
                    save_frame_as_jpeg(annotatedFrame, frameNum, outputDirPath);
                    // std::cout << "  Frame " << frameNum << ": " << frameDetections.size() << " detecção(ões)" << std::endl;
                }
            }
            
            // Perguntar se deseja continuar apenas quando há detecções
            if (currentFrame < totalFrames)
            {
                std::cout << "\nDeseja continuar processando? (s/n): ";
                std::string response;
                std::getline(std::cin, response);

                if (response != "s" && response != "S" && response != "sim" && response != "Sim")
                {
                    std::cout << "Processamento interrompido pelo usuario." << std::endl;
                    break;
                }
            }
        }
        else
        {
            // Sem detecções: prosseguir automaticamente
            if (currentFrame < totalFrames)
            {
                std::cout << "Nenhuma detecção neste batch. Prosseguindo automaticamente...\n" << std::endl;
            }
        }

        // Verificar se todos os frames foram processados
        if (currentFrame >= totalFrames)
        {
            std::cout << "\nTodos os frames foram processados!" << std::endl;
        }
    }

    cap.release();
    std::cout << "\nProcessamento concluido. Frames salvos em: " << outputDirPath << "/" << std::endl;

    return 0;
}