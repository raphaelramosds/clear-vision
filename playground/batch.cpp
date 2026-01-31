#define BATCH_SIZE 128
#define MINI_BATCH_SIZE 4

#include <iostream>
#include <opencv2/opencv.hpp>
#include "processor.h"

int main(int argc, char* argv[])
{
    // Verificar se o caminho do vídeo foi fornecido
    if (argc < 2)
    {
        std::cerr << "Uso: " << argv[0] << " <caminho_do_video>" << std::endl;
        std::cerr << "Exemplo: " << argv[0] << " video_rua01.mp4" << std::endl;
        return 1;
    }

    // Configuração
    BatchProcessorConfig config;
    config.videoPath = argv[1];
    config.onnxPath = "yolov8n.onnx";
    config.classNamesPath = "../core/coco.names";
    config.outputBaseDir = "output";
    config.batchSize = BATCH_SIZE;
    config.miniBatchSize = MINI_BATCH_SIZE;

    // Inicializar modelo e classes
    cv::dnn::Net net;
    std::vector<std::string> classNames;
    std::string errorMsg;
    
    if (!initModelAndClasses(config.onnxPath, config.classNamesPath, net, classNames, errorMsg))
    {
        std::cerr << "Erro ao inicializar: " << errorMsg << std::endl;
        return 1;
    }
    
    std::cout << "Carregadas " << classNames.size() << " classes" << std::endl;

    // Perguntar qual objeto o usuário deseja buscar
    std::cout << "\nDigite o nome do objeto que deseja buscar (ex: person, car, dog): ";
    std::string objectQuery;
    std::getline(std::cin, objectQuery);

    int targetClassId = findClassId(objectQuery, classNames);
    if (targetClassId == -1)
    {
        std::cerr << "Objeto '" << objectQuery << "' nao encontrado nas classes COCO" << std::endl;
        return 1;
    }

    std::cout << "Buscando por: " << classNames[targetClassId] << " (ID " << targetClassId << ")" << std::endl;

    // Criar diretório de saída
    std::string outputDirPath;
    try
    {
        outputDirPath = createOutputDirectory(config.outputBaseDir, classNames[targetClassId]);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Erro ao criar diretório: " << e.what() << std::endl;
        return 1;
    }

    // Abrir vídeo para obter informações
    cv::VideoCapture cap(config.videoPath);
    if (!cap.isOpened())
    {
        std::cerr << "Erro ao abrir video: " << config.videoPath << std::endl;
        return 1;
    }
    
    unsigned int totalFrames = static_cast<unsigned int>(cap.get(cv::CAP_PROP_FRAME_COUNT));
    cap.release();

    std::cout << "Total de frames no video: " << totalFrames << std::endl;
    std::cout << "Processando em batches de " << BATCH_SIZE << " frames\n" << std::endl;

    // Callback para interação com usuário após cada batch
    auto onBatchProcessed = [&classNames, targetClassId](int batchNum, int detectionCount, 
                                                         int savedFrames, int startFrame, 
                                                         int endFrame, unsigned int totalFrames) -> bool
    {
        std::cout << "\n=== Batch " << batchNum << " ===" << std::endl;
        std::cout << "Frames processados: " << startFrame << " a " << endFrame 
                  << " (" << (endFrame + 1) << "/" << totalFrames << ")" << std::endl;
        std::cout << "Deteccoes de '" << classNames[targetClassId] << "': " << detectionCount << std::endl;
        
        if (detectionCount > 0)
        {
            std::cout << "Salvando " << savedFrames << " frames..." << std::endl;
            
            // Perguntar se deseja continuar
            std::cout << "\nDeseja continuar processando? (s/n): ";
            std::string response;
            std::getline(std::cin, response);

            if (response != "s" && response != "S" && response != "sim" && response != "Sim")
            {
                std::cout << "Processamento interrompido pelo usuario." << std::endl;
                return false;
            }
        }
        else
        {
            std::cout << "Nenhuma detecção neste batch. Prosseguindo automaticamente...\n" << std::endl;
        }
        
        return true;
    };

    // Processar vídeo
    bool success = processVideo(config, targetClassId, classNames, net, outputDirPath, onBatchProcessed);

    if (success)
    {
        std::cout << "\nProcessamento concluido. Frames salvos em: " << outputDirPath << "/" << std::endl;
        return 0;
    }
    else
    {
        std::cerr << "\nErro durante o processamento do video." << std::endl;
        return 1;
    }
}