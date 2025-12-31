'use client'

import { useState, useRef } from "react"
import {
  Box,
  Typography,
  Button,
  Paper,
  Stack,
  LinearProgress,
  Alert,
  TextField,
  Divider
} from "@mui/material"
import CloudUploadIcon from "@mui/icons-material/CloudUpload"
import { FileMetadata } from "../types"

export default function VideoUploader() {
  const [uploadedFile, setUploadedFile] = useState<FileMetadata | null>(null)
  const [uploading, setUploading] = useState(false)
  const [error, setError] = useState<string | null>(null)
  const [userPrompt, setUserPrompt] = useState("")
  const [frames, setFrames] = useState<{ src: string; time: number }[]>([])

  const videoRef = useRef<HTMLVideoElement | null>(null)

  const handleFilePicked = (e: React.ChangeEvent<HTMLInputElement>) => {
    const file = e.target.files?.[0]
    if (!file) return

    if (!/\.(mp4|mkv)$/i.test(file.name)) {
      setError("Por favor, selecione um vídeo válido (mp4 ou mkv).")
      setUploadedFile(null)
      return
    }

    setError(null)
    setUploading(true)

    setTimeout(() => {
      setUploadedFile({
        name: file.name,
        size: file.size / 1000,
        type: file.type,
        result: URL.createObjectURL(file)
      })
      setUploading(false)
    }, 1200)
  }

  const handleExtractFrames = async () => {
    if (!videoRef.current) return
    const video = videoRef.current
    const totalDuration = video.duration
    const canvas = document.createElement("canvas")
    const ctx = canvas.getContext("2d")
    if (!ctx) return

    const numFrames = 5
    const step = totalDuration / (numFrames + 1)
    const extracted: { src: string; time: number }[] = []

    for (let i = 1; i <= numFrames; i++) {
      const time = i * step
      await new Promise<void>((resolve) => {
        video.currentTime = time
        video.onseeked = () => {
          canvas.width = video.videoWidth / 3
          canvas.height = video.videoHeight / 3
          ctx.drawImage(video, 0, 0, canvas.width, canvas.height)
          const dataUrl = canvas.toDataURL("image/jpeg")
          extracted.push({ src: dataUrl, time })
          resolve()
        }
      })
    }
    setFrames(extracted)
  }

  return (
    <Box
      display="flex"
      justifyContent="center"
      alignItems="center"
      minHeight="100vh"
      sx={{
        background:
          "linear-gradient(135deg, #0d1117 0%, #0b1720 80%)",
        color: "#e0e0e0",
      }}
    >
      <Paper
        elevation={6}
        sx={{
          p: 5,
          width: "min(90%, 700px)",
          borderRadius: 4,
          background: "#161b22",
          color: "#f0f0f0",
          border: "1px solid rgba(255,255,255,0.1)"
        }}
      >
        <Box display="flex" justifyContent="center" mb={3}>
          <img
            src="/clear-vision-logo.png"
            alt="Clear Vision"
            style={{
              width: "180px",
              height: "auto",
              filter: "drop-shadow(0 0 8px rgba(255,255,255,0.15))",
            }}
          />
        </Box>
        <Typography
          variant="subtitle1"
          sx={{ color: "#aab1bb" }}
          textAlign="center"
          mb={3}
        >
          Extraia insights visuais de vídeos de forma simples
        </Typography>

        <Divider sx={{ mb: 3, borderColor: "rgba(255,255,255,0.1)" }} />

        <Stack spacing={2} alignItems="center">
          <Button
            variant="contained"
            component="label"
            startIcon={<CloudUploadIcon />}
            sx={{
              width: "100%",
              py: 1.5,
              fontWeight: 600,
              textTransform: "none",
              backgroundColor: "#238636",
              "&:hover": { backgroundColor: "#2ea043" }
            }}
          >
            Escolher vídeo
            <input type="file" hidden accept="video/*" onChange={handleFilePicked} />
          </Button>

          {uploading && <LinearProgress sx={{ width: "100%" }} />}

          {error && <Alert severity="error" sx={{ width: "100%" }}>{error}</Alert>}

          {uploadedFile && (
            <>
              <Typography variant="body2" sx={{ color: "#8b949e" }}>
                {uploadedFile.name} — {uploadedFile.size.toFixed(1)} KB
              </Typography>

              <TextField
                label="Descreva o evento a identificar no vídeo"
                fullWidth
                variant="filled"
                InputLabelProps={{ style: { color: "#aab1bb" } }}
                InputProps={{
                  style: {
                    backgroundColor: "#0d1117",
                    color: "#e6edf3",
                  },
                }}
                value={userPrompt}
                onChange={(e) => setUserPrompt(e.target.value)}
                placeholder="Ex: 'Pessoa atravessando a rua'"
              />

              <Box sx={{ mt: 2 }}>
                <video
                  ref={videoRef}
                  src={uploadedFile.result}
                  controls
                  style={{
                    width: "100%",
                    borderRadius: 8,
                    backgroundColor: "#000"
                  }}
                />
              </Box>

              <Button
                variant="outlined"
                onClick={handleExtractFrames}
                disabled={!uploadedFile || uploading}
                sx={{
                  width: "100%",
                  mt: 2,
                  textTransform: "none",
                  fontWeight: 600,
                  borderColor: "#58a6ff",
                  color: "#58a6ff",
                  "&:hover": {
                    borderColor: "#79c0ff",
                    backgroundColor: "rgba(88,166,255,0.1)"
                  }
                }}
              >
                Buscar evento
              </Button>

              {frames.length > 0 && (
                <Box mt={3}>
                  <Typography variant="subtitle1" gutterBottom sx={{ color: "#c9d1d9" }}>
                    Frames representativos — evento:{" "}
                    <strong>{userPrompt || "não especificado"}</strong>
                  </Typography>

                  <Box
                    sx={{
                      display: "flex",
                      gap: 2,
                      overflowX: "auto",
                      scrollSnapType: "x mandatory",
                      p: 1,
                      borderRadius: 2,
                      backgroundColor: "#0d1117",
                      scrollbarWidth: "thin",
                      "&::-webkit-scrollbar": {
                        height: 6
                      },
                      "&::-webkit-scrollbar-thumb": {
                        backgroundColor: "#30363d",
                        borderRadius: 3
                      }
                    }}
                  >
                    {frames.map((frame, idx) => (
                      <Box
                        key={idx}
                        sx={{
                          flex: "0 0 auto",
                          scrollSnapAlign: "start",
                          textAlign: "center"
                        }}
                      >
                        <img
                          src={frame.src}
                          alt={`Frame ${idx}`}
                          style={{
                            width: 160,
                            height: "auto",
                            borderRadius: 8,
                            boxShadow: "0 2px 10px rgba(0,0,0,0.5)"
                          }}
                        />
                        <Typography variant="caption" display="block" mt={0.5} sx={{ color: "#8b949e" }}>
                          {frame.time.toFixed(1)}s
                        </Typography>
                      </Box>
                    ))}
                  </Box>
                </Box>
              )}
            </>
          )}
        </Stack>
      </Paper>
    </Box>
  )
}
