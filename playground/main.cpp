#include <iostream>
#include <opencv2/opencv.hpp>

void loadNet(const char *onnxPath, cv::dnn::Net &net)
{
    net = cv::dnn::readNetFromONNX(onnxPath);
    net.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
    net.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA_FP16);
}

void sampleFrames(const char *videoPath, int n, std::vector<cv::Mat> &frames)
{
    cv::VideoCapture cap(videoPath);
    if (!cap.isOpened())
    {
        throw std::invalid_argument(std::string("Video file does not exist: ") + videoPath);
    }

    unsigned int totalFrames = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_COUNT)),
                 i = 0;

    printf("Existem %ld frames\n", totalFrames);

    if (n < 0 && n != -1)
    {
        std::runtime_error("Invalid argument n");
    }

    int numSamples = n;

    // Extrair todos os frames caso n seja -1
    if (n == -1)
    {
        numSamples = totalFrames;
        printf("ATENCAO: Todos os %ld frames vao ser amostrados\n", numSamples);
    }

    for (; i < numSamples; ++i)
    {
        cv::Mat frame;
        cap.read(frame);
        frames.emplace_back(frame);
    }
    cap.release();

    printf("%d frames inseridos\n", frames.size());
}

void processBatch(std::vector<cv::Mat> &batch, cv::dnn::Net &net)
{
    int inputWidth = 640;
    int inputHeight = 640;
    float scale = 1.0 / 255.0; // Normalizar os pixels de 0 a 255 para 0 a 1.0
    cv::Scalar mean = cv::Scalar(0, 0, 0);
    bool swapRB = true;
    bool crop = false;

    // Criar o blob 4D a partir do vector de frames
    // O OpenCV criará automaticamente o tensor 4x3x640x640
    cv::Mat blob = cv::dnn::blobFromImages(
        batch,
        scale,
        cv::Size(inputWidth, inputHeight),
        mean,
        swapRB,
        crop);

    // Definir a entrada da rede
    net.setInput(blob);

    // Executar o Forward (Inferência)
    // A saída será o tensor 4x84x8400
    cv::Mat output = net.forward();

    // Entendendo a saída
    // output.dims será 3 (ou 4 dependendo da versão do OpenCV)
    // Para acessar cada imagem do batch individualmente:
    int batchSize = output.size[0];      // 4
    int numAttributes = output.size[1];  // 84 (cx, cy, w, h + 80 classes)
    int numPredictions = output.size[2]; // 8400

    std::cout << "Batch processado. Formato da saída: "
              << batchSize << "x" << numAttributes << "x" << numPredictions << std::endl;

    // Exemplo: Acessar os dados da primeira imagem no batch
    // cv::Mat firstImageResult(numAttributes, numPredictions, CV_32F, output.ptr<float>(0));
}

template <typename Func, typename... Args>
void TIMEIT(std::string label, Func func, Args &&...args)
{
    auto start = std::chrono::high_resolution_clock::now();

    // Executa a função passando os argumentos
    std::invoke(func, std::forward<Args>(args)...);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << label << elapsed.count() << "s" << std::endl;
}

int main(int argv, char **argc)
{
    const char *videoPath = "canal1-1min.mp4";
    const char *onnxPath = "yolov8n.onnx";

    // Carregar rede
    cv::dnn::Net net;
    TIMEIT("loadNet: ", loadNet, onnxPath, net);

    // Amostrar N frames
    std::vector<cv::Mat> frames = {};
    int N = 12;
    sampleFrames(videoPath, N, frames);
    std::vector<cv::Mat> batch(frames.begin(), frames.begin() + 4);
    printf("Bach possui %d frames\n", batch.size());

    // Inserir e processar um batch na rede neural
    processBatch(batch, net);

    // Amostrar muitos frames (estouro de stack)
    frames.clear();
    TIMEIT("sampleFrames: ", sampleFrames, videoPath, 1000, frames);

    return 0;
}