import { Paper, Typography, Container } from "@mui/material";

export default function Video({ params }: { params: { video_uid: string } }) {
    const { video_uid } = params;

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
                <Typography variant="h5" fontWeight={600}>
                    Perguntas do Vídeo
                </Typography>

                <Typography variant="body1" sx={{ mt: 2 }}>
                    Video UID: <strong>{video_uid}</strong>
                </Typography>
            </Paper>
        </>
    );
}
