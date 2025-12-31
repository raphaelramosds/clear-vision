"use client";

import { Paper, Typography, Stack, CircularProgress, Box } from "@mui/material";

export default function Dashboard() {
    const storageUsed = "100 MB";
    const storageLimit = "1 GB";

    const inferencesInProgress = [
        { id: 1, target: "Detectando \"homem com camisa preta\" no vídeo A" },
        { id: 2, target: "Detectando \"mulher\" no vídeo B" },
        { id: 3, target: "Detectando \"animais\" no vídeo C" }
    ];

    const inferencesCompleted = 4;

    return (
        <Stack spacing={4}>

            <Stack direction="row" spacing={4}>
                <Paper
                    elevation={4}
                    sx={{
                        flex: 1,
                        p: 3,
                        bgcolor: "#0d1117",
                        border: "1px solid #30363d",
                        color: "#e6edf3",
                    }}
                >
                    <Typography variant="h6" sx={{ mb: 2 }}>
                        Espaço utilizado pelos seus vídeos
                    </Typography>

                    <Typography variant="h4">
                        <Box component="span" sx={{ color: "#58a6ff" }}>
                            {storageUsed}
                        </Box>
                        <Box component="span" sx={{ color: "#8b949e", ml: 1 }}>
                            / {storageLimit}
                        </Box>
                    </Typography>
                </Paper>

                <Paper
                    elevation={4}
                    sx={{
                        flex: 1,
                        p: 3,
                        bgcolor: "#0d1117",
                        border: "1px solid #30363d",
                        color: "#e6edf3",
                    }}
                >
                    <Typography variant="h6" sx={{ mb: 1 }}>
                        Detecções concluídas
                    </Typography>

                    <Typography variant="h4" sx={{ color: "#3fb950" }}>
                        {inferencesCompleted}
                    </Typography>
                </Paper>
            </Stack>

            <Paper
                elevation={4}
                sx={{
                    p: 3,
                    bgcolor: "#0d1117",
                    border: "1px solid #30363d",
                    color: "#e6edf3",
                }}
            >
                <Typography variant="h6" sx={{ mb: 2 }}>
                    Inferências em andamento
                </Typography>

                <Stack spacing={2}>
                    {inferencesInProgress.map((inf) => (
                        <Box
                            key={inf.id}
                            sx={{
                                p: 2,
                                border: "1px solid #30363d",
                                gap: 2,
                                borderRadius: 2,
                                bgcolor: "#161b22",
                                display: "flex",
                                alignItems: "center",
                            }}
                        >
                            <CircularProgress size={20} />
                            <Typography>{inf.target}</Typography>
                        </Box>
                    ))}
                </Stack>
            </Paper>

        </Stack>
    );
}
