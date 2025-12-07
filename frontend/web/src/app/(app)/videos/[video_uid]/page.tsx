"use client";

import { use, useEffect, useState } from "react";
import {
    Paper,
    Typography,
    TextField,
    Button,
    Stack,
    CircularProgress
} from "@mui/material";

import InferencesGatewayHttp from "@infra/api/inferences/inferencesGatewayHttp";

import VideosGatewayHttp from "@infra/api/videos/videosGatewayHttp";
import { useRouter } from "next/navigation";

export default function Video({ params }: { params: { video_uid: string } }) {
    const { video_uid } = use(params);
    const [video, setVideo] = useState<any>(null);

    const router = useRouter();

    const [target, setTarget] = useState("");
    const [loading, setLoading] = useState(false);
    const [result, setResult] = useState<any>(null);
    const [error, setError] = useState("");


    useEffect(() => {
        const loadVideo = async () => {
            const response = await videosGateway.getVideo(video_uid);
            setVideo(response.content);
        };

        loadVideo();

        console.log(video)
    }, [video_uid]);

    const inferencesGateway = new InferencesGatewayHttp();

    const videosGateway = new VideosGatewayHttp();

    const handleCreateInference = async () => {
        if (!target.trim()) return;

        setLoading(true);
        setError("");
        setResult(null);

        try {
            const response = await inferencesGateway.addInference(video_uid, target);
            setResult(response);
        } catch (err: any) {
            setError(err.message);
        }

        setLoading(false);
    };

    return (
        <>
            <Stack direction="row" justifyContent="flex-end" sx={{ mb: 2 }}>
                <Button
                    variant="outlined"
                    onClick={() => router.push("/videos")}
                    sx={{
                        borderColor: "#30363d",
                        color: "#e6edf3",
                        ":hover": {
                            borderColor: "#58a6ff",
                            color: "#58a6ff",
                        }
                    }}
                >
                    Voltar
                </Button>
            </Stack>

            <Paper
                elevation={4}
                sx={{
                    p: 3,
                    mb: 4,
                    bgcolor: "#0d1117",
                    border: "1px solid #30363d",
                    color: "#e6edf3",
                }}
            >
                {video && video.thumbnail && (
                    <Paper
                        elevation={3}
                        sx={{
                            p: 2,
                            mb: 3,
                            bgcolor: "#161b22",
                            border: "1px solid #30363d",
                            color: "#e6edf3",
                        }}
                    >
                        <img
                            src={`data:image/jpeg;base64,${video.thumbnail}`}
                            alt="Thumbnail"
                            style={{
                                width: "100%",
                                maxHeight: "300px",
                                objectFit: "cover",
                                borderRadius: 8,
                                border: "1px solid #30363d"
                            }}
                        />
                    </Paper>
                )}

                <Stack direction="row" spacing={2} sx={{ mt: 3 }}>
                    <TextField
                        label="O que você quer encontrar no vídeo?"
                        variant="outlined"
                        fullWidth
                        value={target}
                        onChange={(e) => setTarget(e.target.value)}
                        sx={{
                            input: { color: "#e6edf3" },
                            label: { color: "#8b949e" },
                            "& .MuiOutlinedInput-root fieldset": {
                                borderColor: "#30363d",
                            },
                        }}
                    />

                    <Button
                        variant="contained"
                        onClick={handleCreateInference}
                        disabled={loading}
                    >
                        Buscar
                    </Button>
                </Stack>

                {loading && (
                    <Stack alignItems="center" sx={{ mt: 3 }}>
                        <CircularProgress size={28} />
                        <Typography sx={{ mt: 1 }}>Processando... </Typography>
                    </Stack>
                )}

                {error && (
                    <Typography color="error" sx={{ mt: 2 }}>
                        {error}
                    </Typography>
                )}

                {result && (
                    <Paper
                        sx={{
                            mt: 3,
                            p: 2,
                            bgcolor: "#161b22",
                            color: "#e6edf3",
                            border: "1px solid #30363d",
                        }}
                    >
                        <Typography variant="subtitle1" fontWeight={500}>
                            Inferência criada com sucesso:
                        </Typography>
                        <pre style={{ marginTop: 8, fontSize: 14 }}>
                            {JSON.stringify(result, null, 2)}
                        </pre>
                    </Paper>
                )}
            </Paper>
        </>
    );
}
