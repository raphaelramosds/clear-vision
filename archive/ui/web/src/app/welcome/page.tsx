import { Box, Typography, Button, Card, CardContent } from "@mui/material";
import Link from "next/link";

export default function Welcome() {
  return (
    <Box
      minHeight="100vh"
      display="flex"
      flexDirection="column"
      justifyContent="center"
      alignItems="center"
      gap={5}
    >
      <Card
        sx={{
          background: "none",
          textAlign: "center",
        }}
      >
        <CardContent>
          <Box display="flex" justifyContent="center" mb={3}>
            <img
              src="/clear-vision-logo.png"
              alt="Clear Vision"
              style={{
                width: "300px",
                height: "auto",
                filter: "drop-shadow(0 0 8px rgba(255,255,255,0.15))",
              }}
            />
          </Box>

          <Typography variant="body2" sx={{ color: "#aab1bb" }} mb={3}>
            Encontre o que quiser em um vídeo com um clique
          </Typography>

          <Button
            component={Link}
            href="/dashboard"
            variant="contained"
            sx={{
              textTransform: "none",
              fontSize: "1rem",
              py: 1.2,
              px: 4,
              borderRadius: 3,
              background: "#c8ab37ff",
              ":hover": { background: "#d6c45aff" },
            }}
          >
            Começar
          </Button>
        </CardContent>
      </Card>
    </Box>
  );
}
