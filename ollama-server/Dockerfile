FROM ollama/ollama:latest

ARG OLLAMA_MODEL

ENV OLLAMA_MODEL=${OLLAMA_MODEL}
ENV OLLAMA_HOST=0.0.0.0

COPY entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh

ENTRYPOINT [ "/entrypoint.sh" ]