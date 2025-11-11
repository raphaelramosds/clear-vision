'use client'

import { useState } from "react";
import {
  Box,
  Typography,
  Button,
  Paper,
  Stack,
  LinearProgress,
  Alert,
  TextField
} from "@mui/material";
import CloudUploadIcon from "@mui/icons-material/CloudUpload";
import { FileMetadata } from "../types";

export default function VideoUploader() {
  const [uploadedFile, setUploadedFile] = useState<FileMetadata | null>(null);
  const [uploading, setUploading] = useState(false);
  const [error, setError] = useState<string | null>(null);
  const [frames, setFrames] = useState<number | null>(null);
  const [userPrompt, setUserPrompt] = useState("");

  const handleFilePicked = (e: React.ChangeEvent<HTMLInputElement>) => {
    const file = e.target.files?.[0];
    if (!file) return;
    setError(null);
    setUploadedFile({
      name: file.name,
      size: file.size / 1000,
      type: file.type,
      result: URL.createObjectURL(file),
    });
  };

  const handleUpload = async () => {
    if (!uploadedFile || !userPrompt) {
      setError("Selecione um vídeo e preencha o prompt antes de enviar.");
      return;
    }

    setUploading(true);
    setError(null);
    setFrames(null);

    try {
      const formData = new FormData();
      formData.append("video", (document.querySelector("input[type='file']") as HTMLInputElement).files![0]);
      formData.append("user_prompt", userPrompt);

      const res = await fetch("http://localhost:4000/clear-vision/v1/upload/", {
        method: "POST",
        body: formData,
      });

      if (!res.ok) {
        const errText = await res.text();
        throw new Error(`Erro ${res.status}: ${errText}`);
      }

      const data = await res.json();
      setFrames(data.num_frames || 0);
    } catch (err: any) {
      setError(err.message || "Erro ao enviar vídeo.");
    } finally {
      setUploading(false);
    }
  };

  return (
    <Box display="flex" justifyContent="center" alignItems="center" minHeight="90vh">
      <Paper elevation={3} sx={{ p: 4, width: 480, textAlign: "center" }}>
        <Typography variant="h5" gutterBottom>
          Upload de Vídeo
        </Typography>

        <Stack spacing={2} alignItems="center">
          <Button
            variant="contained"
            component="label"
            startIcon={<CloudUploadIcon />}
            sx={{ width: "100%" }}
          >
            Escolher vídeo
            <input type="file" hidden accept="video/*" onChange={handleFilePicked} />
          </Button>

          {uploadedFile && (
            <Typography variant="body2" color="text.secondary">
              {uploadedFile.name} — {uploadedFile.size.toFixed(1)} KB
            </Typography>
          )}

          <TextField
            label="Prompt do usuário"
            fullWidth
            value={userPrompt}
            onChange={(e) => setUserPrompt(e.target.value)}
            placeholder="Descreva o que deseja analisar no vídeo..."
          />

          <Button
            variant="outlined"
            onClick={handleUpload}
            disabled={!uploadedFile || uploading}
            sx={{ width: "100%" }}
          >
            {uploading ? "Enviando..." : "Fazer upload"}
          </Button>

          {uploading && <LinearProgress sx={{ width: "100%" }} />}

          {error && <Alert severity="error" sx={{ width: "100%" }}>{error}</Alert>}

          {frames !== null && !uploading && (
            <Alert severity="success" sx={{ width: "100%" }}>
              ✅ O vídeo possui <strong>{frames}</strong> frames.
            </Alert>
          )}

          {uploadedFile?.result && (
            <Box mt={2}>
              <video
                src={uploadedFile.result}
                controls
                style={{
                  width: "100%",
                  borderRadius: 8,
                  backgroundColor: "#000",
                }}
              />
            </Box>
          )}
        </Stack>
      </Paper>
    </Box>
  );
}
