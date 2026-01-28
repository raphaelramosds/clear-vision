#include <iostream>

struct Detection
{
    int classId;
    int frameNumber;
    float x, y, w, h;
    float confidence;
};

template <typename Func, typename... Args>
static void TIMEIT(std::string label, Func func, Args &&...args)
{
    auto start = std::chrono::high_resolution_clock::now();

    // Executa a função passando os argumentos
    std::invoke(func, std::forward<Args>(args)...);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << label << elapsed.count() << "s" << std::endl;
}

inline void loadNet(const char *onnxPath, cv::dnn::Net &net)
{
    net = cv::dnn::readNetFromONNX(onnxPath);
    net.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
    net.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA_FP16);

    if (net.empty())
    {
        throw std::runtime_error("Model not loaded");
    }
}

inline void sampleFrames(const char *videoPath, int n, std::vector<cv::Mat> &frames)
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

inline void processBatch(std::vector<cv::Mat> &batch, cv::dnn::Net &net, int startFrame , std::vector<Detection> &detections)
{
    float confidenceThreshold = 0.5;
    float nmsThreshold = 0.4;
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
    std::vector<cv::Mat> outputs;
    net.forward(outputs, net.getUnconnectedOutLayersNames());

    cv::Mat output = outputs.front();

    // Entendendo a saída
    // output.dims será 3 (ou 4 dependendo da versão do OpenCV)
    // Para acessar cada imagem do batch individualmente:
    int batchSize = output.size[0];      // 4
    int numAttributes = output.size[1];  // 84 (cx, cy, w, h + 80 classes)
    int numPredictions = output.size[2]; // 8400

    std::cout << "Batch processado. Formato da saída: "
              << batchSize << "x" << numAttributes << "x" << numPredictions << std::endl;

    for (int b = 0; b < batchSize; ++b)
    {
        std::vector<int> classIds = {};
        std::vector<float> confidences = {};
        std::vector<cv::Rect> boxes = {};

        float xScale = batch[0].cols / static_cast<float>(inputWidth);
        float yScale = batch[0].rows / static_cast<float>(inputHeight);
        float x, y, w, h;

        float *batchData = output.ptr<float>() + (b * numAttributes * numPredictions);
        cv::Mat row = cv::Mat(numAttributes, numPredictions, CV_32F, batchData);
        cv::Mat outputReshaped = row.t();

        for (int i = 0; i < numPredictions; ++i)
        {
            const float *ptr = outputReshaped.ptr<float>(i);
            int classId = 0;
            float maxProb = 0.0f;

            // Find class with max probability (indices 4-83)
            for (int c = 4; c < numAttributes; ++c)
            {
                // float prob = output.at<float>(0, c, i);
                float prob = ptr[c];

                if (prob > maxProb)
                {
                    maxProb = prob;
                    classId = c - 4; // Convert to class ID 0-79
                }
            }

            // Use max class probability as confidence
            if (maxProb >= confidenceThreshold)
            {
                // x = output.at<float>(0, 0, i) * xScale;
                // y = output.at<float>(0, 1, i) * yScale;
                // w = output.at<float>(0, 2, i) * xScale;
                // h = output.at<float>(0, 3, i) * yScale;

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