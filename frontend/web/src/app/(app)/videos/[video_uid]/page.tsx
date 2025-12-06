"use client";

import { use, useState } from "react";
import {
    Paper,
    Typography,
    TextField,
    Button,
    Stack,
    CircularProgress
} from "@mui/material";

import InferencesGatewayHttp from "@infra/api/inferences/inferencesGatewayHttp";

export default function Video({ params }: { params: { video_uid: string } }) {
    const { video_uid } = use(params);

    const [target, setTarget] = useState("");
    const [loading, setLoading] = useState(false);
    const [result, setResult] = useState<any>(null);
    const [error, setError] = useState("");

    const gateway = new InferencesGatewayHttp();

    const handleCreateInference = async () => {
        if (!target.trim()) return;

        setLoading(true);
        setError("");
        setResult(null);

        try {
            const response = await gateway.addInference(video_uid, target);
            setResult(response);
        } catch (err: any) {
            setError(err.message);
        }

        setLoading(false);
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
                <Typography variant="h5" fontWeight={600}>
                    Perguntas do Vídeo
                </Typography>

                <Typography variant="body1" sx={{ mt: 2 }}>
                    Video UID: <strong>{video_uid}</strong>
                </Typography>

                <Stack direction="row" spacing={2} sx={{ mt: 3 }}>
                    <TextField
                        label="Descrição do alvo (target)"
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
                        Criar
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
