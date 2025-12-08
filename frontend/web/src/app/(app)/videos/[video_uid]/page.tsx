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
    const [inferences, setInferences] = useState<any[]>([]);


    useEffect(() => {
        const loadVideo = async () => {
            const response = await videosGateway.getVideo(video_uid);
            setVideo(response.content);
        };

        const loadInferences = async () => {
            const response = await videosGateway.getVideoInferences(video_uid);
            setInferences(response.content || []);
        };

        loadVideo();
        loadInferences();
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

                {inferences.length > 0 && (
                    <Stack
                        // maxWidth={700}
                        spacing={3}
                        sx={{
                            mt: 4,
                        }}
                    >
                        {inferences.map((inference) => (
                            <Paper
                                key={inference.uid}
                                elevation={3}
                                sx={{
                                    p: 2,
                                    bgcolor: "#161b22",
                                    border: "1px solid #30363d",
                                    color: "#e6edf3",
                                    overflow: "auto",
                                    "&::-webkit-scrollbar": {
                                        height: "8px",
                                    },
                                    "&::-webkit-scrollbar-thumb": {
                                        backgroundColor: "#30363d",
                                        borderRadius: "4px",
                                    },
                                    "&::-webkit-scrollbar-track": {
                                        backgroundColor: "#161b22",
                                    },
                                }}
                            >
                                <Typography variant="h6" sx={{ mb: 2 }}>
                                    {inference.target}
                                </Typography>

                                <Stack direction="row">
                                    {inference.timestamps.map((ts: string, i: number) => (
                                        <Paper
                                            key={i}
                                            elevation={1}
                                            sx={{
                                                p: 1,
                                                border: "2px solid yellow",
                                                borderRadius: 1,
                                                bgcolor: "#0d1117",
                                                minWidth: "60px",
                                                textAlign: "center",
                                                color: 'white'
                                            }}
                                        >
                                            <Typography>{ts}s</Typography>
                                        </Paper>
                                    ))}
                                </Stack>
                            </Paper>
                        ))}
                    </Stack>
                )}
            </Paper>
        </>
    );
}
