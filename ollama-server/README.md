Start ollama server

    docker compose up -d --build

Pull LLM model

    docker exec -it ollama-server-server-1 ollama pull $OLLAMA_MODEL

Run LLM model

    docker exec -it ollama-server-server-1 ollama run $OLLAMA_MODEL