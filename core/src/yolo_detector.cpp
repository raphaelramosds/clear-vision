#include "yolo_detector.hpp"
#include <opencv2/opencv.hpp>

YOLODetector::YOLODetector(YOLOModel &_model)
    : model(_model)
{
}

std::vector<cvision::Detection> YOLODetector::detectObjects(cv::Mat &frame)
{
    std::vector<cvision::Detection> detections;
    cv::dnn::Net &net = model.getNet();
    int inputWidth = model.getInputWidth();
    int inputHeight = model.getInputHeight();
    float confidenceThreshold = model.getConfidenceThreshold();
    float nmsThreshold = model.getNmsThreshold();
    if (net.empty())
    {
        std::cerr << "Model not loaded" << std::endl;
        return detections;
    }

    // Prepare blob from frame
    cv::Mat blob = cv::dnn::blobFromImage(
        frame, 1.0 / 255.0,
        cv::Size(inputWidth, inputHeight),
        cv::Scalar(), true, false);

    net.setInput(blob);

    // size_t weightsMemory = 0;
    // size_t blobsMemory = 0;
    // std::vector<int> shape(blob.size.p, blob.size.p + blob.dims);
    // net.getMemoryConsumption(shape, weightsMemory, blobsMemory);
    // double totalMemoryMB = (weightsMemory + blobsMemory) / (1024.0 * 1024.0);
    // std::cout << "Total memory consumption: " << std::fixed << std::setprecision(2) << totalMemoryMB << " MB" << std::endl;
    
    std::vector<cv::Mat> outputs;
    net.forward(outputs, net.getUnconnectedOutLayersNames());

    // Process outputs
    std::vector<int> classIds;
    std::vector<float> confidences;
    std::vector<cv::Rect> boxes;

    float xScale = frame.cols / static_cast<float>(inputWidth);
    float yScale = frame.rows / static_cast<float>(inputHeight);
    float x,y,w,h;

    for (const auto &output : outputs)
    {
        // FIXME will break for other YOLO versions (like YOLOv5, v6, v7)

        // Expect a YOLOv8 tensor [1, 84, 8400] output format.
        // Indices 0-3: x, y, w, h
        // Indices 4-83: class probabilities (no separate confidence)
        int numBoxes = output.size[2]; // 8400
        int numAttributes = output.size[1]; // 84 for YOLOv8

        // Transpose output to shape [1, boxes = 84000, attributes = 84] for better cache access
        cv::Mat outputReshaped = output.reshape(1, numAttributes).t();
        
        for (int i = 0; i < numBoxes; ++i)
        {
            const float* ptr = outputReshaped.ptr<float>(i);
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
    }

    // Apply Non-Maximum Suppression
    std::vector<int> indices;
    cv::dnn::NMSBoxes(boxes, confidences, confidenceThreshold, nmsThreshold, indices);

    // Create detection results
    for (int idx : indices)
    {
        cvision::Detection det;
        det.x = boxes[idx].x;
        det.y = boxes[idx].y;
        det.w = boxes[idx].width;
        det.h = boxes[idx].height;
        det.confidence = confidences[idx];
        det.classId = classIds[idx];
        det.className = model.getClassName(classIds[idx]);
        detections.push_back(det);
    }

    return detections;
}

std::vector<std::vector<cvision::Detection>> YOLODetector::detectObjectsBatch(const std::vector<cv::Mat> &frames)
{
    std::vector<std::vector<cvision::Detection>> allDetections;
    cv::dnn::Net &net = model.getNet();
    int inputWidth = model.getInputWidth();
    int inputHeight = model.getInputHeight();
    float confidenceThreshold = model.getConfidenceThreshold();
    float nmsThreshold = model.getNmsThreshold();
    
    if (net.empty())
    {
        std::cerr << "Model not loaded" << std::endl;
        return allDetections;
    }

    if (frames.empty())
    {
        std::cerr << "No frames provided" << std::endl;
        return allDetections;
    }

    // Prepare blob from frames (creates batch automatically)
    // Input: Nx3x640x640 where N = frames.size()
    cv::Mat blob = cv::dnn::blobFromImages(
        frames, 1.0 / 255.0,
        cv::Size(inputWidth, inputHeight),
        cv::Scalar(), true, false);

    net.setInput(blob);
    
    std::vector<cv::Mat> outputs;
    net.forward(outputs, net.getUnconnectedOutLayersNames());

    // Initialize detection vectors for each frame
    int batchSize = frames.size();
    allDetections.resize(batchSize);

    for (const auto &output : outputs)
    {
        // Output format: [batchSize=N, numAttributes=84, numBoxes=8400]
        int outputBatchSize = output.size[0];
        int numAttributes = output.size[1];   // 84
        int numBoxes = output.size[2];        // 8400

        // Process each image in the batch
        for (int b = 0; b < outputBatchSize; ++b)
        {
            std::vector<int> classIds;
            std::vector<float> confidences;
            std::vector<cv::Rect> boxes;

            float xScale = frames[b].cols / static_cast<float>(inputWidth);
            float yScale = frames[b].rows / static_cast<float>(inputHeight);

            // Extract data for this batch element [84, 8400]
            cv::Range ranges[3] = {cv::Range(b, b+1), cv::Range::all(), cv::Range::all()};
            cv::Mat batchOutput = output(ranges).reshape(1, numAttributes);
            
            // Transpose to [8400, 84] for better cache access
            cv::Mat outputReshaped = batchOutput.t();
            
            for (int i = 0; i < numBoxes; ++i)
            {
                const float* ptr = outputReshaped.ptr<float>(i);
                int classId = 0;
                float maxProb = 0.0f;

                // Find class with max probability (indices 4-83)
                for (int c = 4; c < numAttributes; ++c)
                {
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
                    float x = ptr[0] * xScale;
                    float y = ptr[1] * yScale;
                    float w = ptr[2] * xScale;
                    float h = ptr[3] * yScale;

                    boxes.push_back(cv::Rect(
                        static_cast<int>(x - w / 2),
                        static_cast<int>(y - h / 2),
                        static_cast<int>(w),
                        static_cast<int>(h)));
                    confidences.push_back(maxProb);
                    classIds.push_back(classId);
                }
            }

            // Apply Non-Maximum Suppression for this frame
            std::vector<int> indices;
            cv::dnn::NMSBoxes(boxes, confidences, confidenceThreshold, nmsThreshold, indices);

            // Create detection results for this frame
            for (int idx : indices)
            {
                cvision::Detection det;
                det.x = boxes[idx].x;
                det.y = boxes[idx].y;
                det.w = boxes[idx].width;
                det.h = boxes[idx].height;
                det.confidence = confidences[idx];
                det.classId = classIds[idx];
                det.className = model.getClassName(classIds[idx]);
                allDetections[b].push_back(det);
            }
        }
    }

    return allDetections;
}

void YOLODetector::drawDetections(cv::Mat &frame, const std::vector<cvision::Detection> &detections)
{
    for (const auto &det : detections)
    {
        // Draw bounding box
        cv::rectangle(frame, cv::Point(det.x, det.y),
                      cv::Point(det.x + det.w, det.y + det.h),
                      cv::Scalar(0, 255, 0), 2);

        // Draw label with confidence
        std::ostringstream label;
        label << det.className << ": " << std::fixed << std::setprecision(2)
              << (det.confidence * 100) << "%";

        int baseLine = 0;
        cv::Size textSize = cv::getTextSize(label.str(),
                                            cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);

        cv::rectangle(frame, cv::Point(det.x, det.y - textSize.height - 5),
                      cv::Point(det.x + textSize.width, det.y),
                      cv::Scalar(0, 255, 0), cv::FILLED);
        cv::putText(frame, label.str(), cv::Point(det.x, det.y - 5),
                    cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0), 1);
    }
}