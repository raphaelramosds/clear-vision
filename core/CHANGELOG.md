# Changelog

Processamento dos frames com múltiplas threads competindo por um único modelo
- O tempo de execução diminui comparação com a versão single-threaded, mas não é uma diminuição significativa
- Região crítica para acesso ao modelo, já que cv::dnn::Net não é thread-safe (não permite acesso concorrente ao modelo, o OpenCV lança exceção se isso ocorrer)
- Escala bem: ao aumentar o número de threads, o tempo de execução diminui proporcionalmente 

Uso de um modelo mais leve (yolov8n) com múltiplas threads, delegando uma cópia do modelo para cada thread. Cada thread responsável por processar um conjunto de frames.
- Não escala: ao aumentar o número de threads, o tempo de execução não diminui proporcionalmente

Exportação do modelo ONNX com mais de um batch: (batch x 3 x 640 x 640) -> (1 x 84 x 640 x 640)
- Preciso explorar mais essa funcionalidade
- Talvez abandone o uso de múltiplas threads

Amostrar todos os frames de um vídeo e guardá-los na memória
- Estouro de memória 
- Pode ser solucionado com uma estratégia de buffer: ter uma opção do usuário clicar em "próxima detecção" ou "detecção anterior"
- Buffer guarda até N detecções
- Evitar carregar todo o vídeo na memória
