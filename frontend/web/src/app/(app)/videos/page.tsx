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
    Paper,
    Grid,
    IconButton,
    Fab
} from "@mui/material";

import AddCircleIcon from "@mui/icons-material/AddCircle";
import { DeleteOutline, UploadFile } from "@mui/icons-material";

interface VideoDTO {
    uid: string;
    filename: string;
    thumbnail: string;
}

export default function Videos() {
    const [videos, setVideos] = useState<VideoDTO[]>([]);
    const [loading, setLoading] = useState(false);
    const [loadingVideos, setLoadingVideos] = useState(false);
    const router = useRouter();
    const videosGateway = new VideosGatewayHttp();

    const loadVideos = async () => {
        setLoadingVideos(true);
        const { content } = await videosGateway.getVideos();
        setVideos(content);
        setLoadingVideos(false);
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
            {/* <Paper
                elevation={4}
                sx={{
                    p: 3,
                    mb: 4,
                    bgcolor: "inherit",
                    border: "none",
                    color: "#e6edf3",
                }}
            >
                <Stack
                    direction="row"
                    justifyContent="space-between"
                    alignItems="center"
                >
                    <Typography variant="h5" fontWeight={600}>
                        Explorador de vídeos
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
            </Paper> */}

            {/* {loading && (
                <Stack alignItems="center" sx={{ mb: 3 }}>
                    <CircularProgress />
                    <Typography variant="body2" sx={{ mt: 1, color: "#e6edf3" }}>
                        Enviando vídeo...
                    </Typography>
                </Stack>
            )} */}

            <Grid container spacing={2} justifyContent="center">
                {videos && !loadingVideos ? videos.map((video) => (
                    <Grid item xs={12} sm={6} md={6} key={video.uid}>
                        <Card
                            elevation={3}
                            onClick={() => goToVideo(video.uid)}
                            sx={{
                                bgcolor: "#161b22",
                                border: "1px solid #30363d",
                                color: "#e6edf3",
                                height: "100%",
                                display: "flex",
                                flexDirection: "column",
                                cursor: "pointer",
                                position: "relative",
                                "&:hover .thumb": {
                                    filter: "brightness(0.7)"
                                }
                            }}
                        >
                            <IconButton
                                onClick={(e) => {
                                    e.stopPropagation()
                                }}
                                sx={{
                                    position: "absolute",
                                    top: 8,
                                    right: 8,
                                    color: "#e6edf3",
                                    "&:hover": { color: "#ff6b6b" }
                                }}
                            >
                                <DeleteOutline />
                            </IconButton>

                            <img
                                className="thumb"
                                src={`data:image/jpeg;base64,${video.thumbnail}`}
                                alt={video.filename}
                                style={{
                                    width: "100%",
                                    height: "230px",
                                    objectFit: "cover",
                                    borderBottom: "1px solid #30363d",
                                    transition: "0.3s"
                                }}
                            />
                        </Card>
                    </Grid>
                )) : (
                    <Stack alignItems="center" sx={{ mb: 3, width: "100%" }}>
                        <CircularProgress />
                    </Stack>
                )}
            </Grid>


            <Fab
                color="primary"
                onClick={handleAddVideo}
                sx={{
                    position: "fixed",
                    bottom: 32,
                    right: 32,
                    bgcolor: "#2663f0",
                    "&:hover": { bgcolor: "#1f4ec9" }
                }}
            >
                <UploadFile />
            </Fab>
        </>
    );
}
