# Clear Vision

## Requirements

Python modules

```bash
pip install --no-cache-dir poetry==2.1.4 torch torchvision 
```

Packages to use cv2 modules

```bash
sudo apt-get install -y --no-install-recommends libgl1 libglib2.0-0 gcc
```

## Development environment

```bash
# Run frontend and database
docker compose up -d

# Go to backend directory
cd backend

# Run API locally (without docker)
poetry instal
source ~/.cache/pypoetry/name-of-the-environment-poetry-created/bin/activate
DYNAMODB_ENDPOINT_URL="http://0.0.0.0:8000" \
    uvicorn clear_vision.entrypoints.api.main:app --host 0.0.0.0 --port 4000 --reload
    
# Tests
DYNAMODB_ENDPOINT_URL="http://0.0.0.0:8005" \
   pytest
```

## 📚 Project Overview

Clear Vision é uma plataforma completa de visão computacional para detecção e rastreamento de objetos em vídeos. O projeto é composto por três componentes principais:

### 🎯 Backend

**Localização:** `/home/rapha/Projetos/clear-vision/backend/`

Backend Python com arquitetura em camadas (Clean Architecture):

**Funcionalidades:**
- ✅ API REST com FastAPI
- ✅ Detecção de objetos em vídeos usando YOLO
- ✅ Armazenamento em DynamoDB
- ✅ Processamento assíncrono
- ✅ Inferências e rastreamento de vídeos

**Estrutura:**
```
backend/
├── clear_vision/
│   ├── domain/          # Lógica de negócio pura
│   ├── adapters/        # Implementações de interfaces
│   ├── entrypoints/     # API REST (FastAPI)
│   ├── infra/           # Configurações DynamoDB
│   └── config/          # Configuração geral
├── tests/               # Suite de testes
├── pyproject.toml       # Dependências Poetry
└── Makefile
```

**Como usar:**
```bash
cd backend
poetry install
DYNAMODB_ENDPOINT_URL="http://0.0.0.0:8000" poetry run uvicorn clear_vision.entrypoints.api.main:app --reload
```

**Endpoints principais:**
- `POST /api/videos` - Upload e processamento de vídeo
- `GET /api/videos/{id}` - Obter detalhes do vídeo
- `POST /api/inferences` - Criar inferências
- `GET /api/inferences/{video_id}` - Obter resultados

---

### 🎬 Video Processor (Video)

**Localização:** `/home/rapha/Projetos/clear-vision/video/`

Aplicação C++ de alta performance para processamento de vídeos com YOLO:

**Funcionalidades:**
- ✅ Leitura de múltiplos formatos de vídeo
- ✅ Detecção de objetos com YOLO (ONNX e Darknet)
- ✅ Aceleração GPU (CUDA)
- ✅ Exportação de resultados em JSON e MP4 anotado
- ✅ Frame skipping para otimização
- ✅ Processamento em tempo real

**Arquitetura Modular:**
```
video/
├── include/
│   ├── yolo-detector.hpp        # Interface YOLODetector
│   └── video-processor.hpp      # Interface VideoProcessor
├── yolo-detector.cpp            # Implementação detector
├── video-processor.cpp          # Implementação processador
├── main.cpp                     # Aplicação CLI
├── CMakeLists.txt               # Build system
└── Makefile
```

**Performance:**
- GPU (RTX 3070): 100-150 FPS (YOLOv8n)
- CPU (i7): 10-15 FPS (YOLOv8n)

**Como usar:**
```bash
cd video
make build
bash download-models.sh
./build/video-processor input.mp4 -m models/yolov8n.onnx -n models/coco.names -o output.mp4
```

**Saídas:**
- MP4 anotado com bounding boxes
- JSON estruturado com detecções por frame
- Métricas de processamento

---

### 🖥️ Frontend

**Localização:** `/home/rapha/Projetos/clear-vision/frontend/`

Aplicação web e desktop moderna com Next.js + Electron:

**Funcionalidades:**
- ✅ Interface web responsiva
- ✅ Upload de vídeos
- ✅ Dashboard com métricas
- ✅ Visualização de detecções
- ✅ Aplicação desktop (Electron)
- ✅ Integração em tempo real com backend

**Estrutura:**
```
frontend/
├── web/                         # Next.js web app
│   ├── src/
│   │   ├── app/                 # Páginas e rotas
│   │   ├── components/          # Componentes React
│   │   ├── infra/               # API gateway
│   │   └── types/               # TypeScript types
│   └── next.config.ts
├── electron/                    # Electron desktop app
│   └── index.ts
├── package.json
└── tsconfig.json
```

**Páginas principais:**
- `/` - Welcome page
- `/dashboard` - Dashboard com vídeos processados
- `/videos` - Galeria de vídeos
- `/videos/[id]` - Detalhes com visualização de detecções

**Como usar:**
```bash
cd frontend

# Web development
npm install
npm run dev
# Acessa: http://localhost:3000

# Desktop (Electron)
npm run dev:electron
```

**Tecnologias:**
- Next.js 14
- TypeScript
- Tailwind CSS
- Electron (desktop)

---

## 🔄 Fluxo da Aplicação

```
1. Usuário acessa frontend (web ou desktop)
   ↓
2. Realiza upload de vídeo
   ↓
3. Frontend envia para backend API
   ↓
4. Backend recebe e coordena processamento
   ↓
5. Invoca video processor (C++)
   ├─ Lê vídeo
   ├─ Executa detecção YOLO
   ├─ Salva MP4 anotado
   └─ Exporta JSON com resultados
   ↓
6. Backend armazena em DynamoDB
   ↓
7. Frontend busca resultados
   ↓
8. Usuário visualiza vídeo com detecções
```

---

## 📊 Comparativo dos Componentes

| Aspecto | Backend | Video | Frontend |
|---------|---------|-------|----------|
| **Linguagem** | Python | C++ | TypeScript/React |
| **Arquitetura** | Clean Arch | Modular | Next.js |
| **Responsabilidade** | Lógica negócio | Processamento | Apresentação |
| **Performance** | Média | Alta (GPU) | Excelente |
| **Escalabilidade** | Horizontal | Vertical | Horizontal |
| **Acesso** | API REST | CLI | Web/Desktop |

---

## 🚀 Deployment

### Local Development
```bash
# Terminal 1: Database + Frontend
docker compose up -d

# Terminal 2: Backend
cd backend && poetry run uvicorn clear_vision.entrypoints.api.main:app --reload

# Terminal 3: Video processing (quando necessário)
cd video && ./build/video-processor ...
```

### Production
```bash
# Docker compose com todos os serviços
docker compose -f docker-compose.prod.yml up -d
```

---

## 📝 Documentação

- **Backend**: [backend/README.md](backend/README.md)
- **Video Processor**: [video/README.md](video/README.md) + [video/QUICKSTART.md](video/QUICKSTART.md)
- **Frontend**: [frontend/docs/README.md](frontend/docs/README.md)

---

**Status:** ✅ Projeto em desenvolvimento ativo