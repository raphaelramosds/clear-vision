"use client";

import { useState, useEffect } from "react";
import { useRouter } from "next/navigation";
import { VideosGatewayHttp } from "../../../infra/api/videos";

import {
    Button,
    Card,
    CardContent,
    CardActions,
    Typography,
    Stack,
    CircularProgress,
    Paper
} from "@mui/material";

import AddCircleIcon from "@mui/icons-material/AddCircle";

interface VideoDTO {
    uid: string;
    filename: string;
}

export default function Videos() {
    const [videos, setVideos] = useState<VideoDTO[]>([]);
    const [loading, setLoading] = useState(false);
    const router = useRouter();
    const videosGateway = new VideosGatewayHttp();

    const loadVideos = async () => {
        const data = await videosGateway.getVideos();
        setVideos(data);
    };

    useEffect(() => {
        loadVideos();
    }, []);

    const handleAddVideo = async () => {
        const input = document.createElement("input");
        input.type = "file";
        input.accept = "video/*";
        input.onchange = async () => {
            const file = input.files?.[0];
            if (!file) return;
            setLoading(true);
            await videosGateway.uploadVideo(file);
            await loadVideos();
            setLoading(false);
        };
        input.click();
    };

    const goToVideo = (uid: string) => {
        router.push(`/videos/${uid}`);
    };

    return (
        <>
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
                <Stack
                    direction="row"
                    justifyContent="space-between"
                    alignItems="center"
                >
                    <Typography variant="h5" fontWeight={600}>
                        Vídeos
                    </Typography>

                    <Button
                        variant="contained"
                        startIcon={<AddCircleIcon />}
                        onClick={handleAddVideo}
                        disableElevation
                        sx={{
                            bgcolor: "#2663f0",
                            "&:hover": { bgcolor: "#1f4ec9" }
                        }}
                    >
                        Adicionar vídeo
                    </Button>
                </Stack>
            </Paper>

            {loading && (
                <Stack alignItems="center" sx={{ mb: 3 }}>
                    <CircularProgress />
                    <Typography variant="body2" sx={{ mt: 1, color: "#e6edf3" }}>
                        Enviando vídeo...
                    </Typography>
                </Stack>
            )}

            <Stack spacing={2}>
                {videos.map((video) => (
                    <Card
                        key={video.uid}
                        elevation={3}
                        sx={{
                            bgcolor: "#161b22",
                            border: "1px solid #30363d",
                            color: "#e6edf3",
                        }}
                    >
                        <CardContent>
                            <Typography variant="h6" fontWeight={500}>
                                {video.filename}
                            </Typography>
                        </CardContent>

                        <CardActions
                            sx={{
                                justifyContent: "flex-end",
                                p: 2,
                            }}
                        >
                            <Button
                                variant="outlined"
                                onClick={() => goToVideo(video.uid)}
                                sx={{
                                    borderColor: "#30363d",
                                    color: "#e6edf3",
                                    "&:hover": {
                                        borderColor: "#58a6ff",
                                        color: "#58a6ff",
                                    },
                                }}
                            >
                                Perguntas
                            </Button>
                        </CardActions>
                    </Card>
                ))}
            </Stack>
        </>
    );
}
