'use client';

import Link from "next/link";
import { Box, Typography } from "@mui/material";
import DashboardIcon from "@mui/icons-material/Dashboard";
import VideoLibraryIcon from "@mui/icons-material/VideoLibrary";
import { usePathname } from "next/navigation";

export default function AppLayout({ children }: { children: React.ReactNode }) {
  const pathname = usePathname();

  const isActive = (route: string) => pathname.startsWith(route);

  return (
    <Box 
        minWidth="100vh"
        minHeight="100vh" 
        display="flex"
        flexDirection="column"
    >
      <Box
        display="flex"
        justifyContent="center"
        alignItems="center"
        gap={6}
        p={1}
        my={3}
        sx={{
          background: "linear-gradient(135deg, #0d1117 0%, #0b1720 80%)",
          borderRadius: 1,
          boxShadow: "0 0 20px rgba(0,0,0,0.45)",
          position: "sticky",
          top: 0,
          zIndex: 10,
        }}
      >
        <Link href="/">
          <img
            src="/clear-vision-icon.png"
            alt="Clear Vision"
            style={{ height: "42px", filter: "drop-shadow(0 0 6px rgba(255,255,255,0.08))" }}
          />
        </Link>

        <Typography
          component={Link}
          href="/dashboard"
          sx={{
            display: "flex",
            alignItems: "center",
            gap: 1,
            textDecoration: "none",
            color: isActive("/dashboard") ? "#fff" : "#9ea6ac",
            fontSize: "1.05rem",
            fontWeight: isActive("/dashboard") ? 600 : 400,
            "&:hover": { color: "#fff" },
          }}
        >
          <DashboardIcon fontSize="small" />
          Dashboard
        </Typography>

        <Typography
          component={Link}
          href="/videos"
          sx={{
            display: "flex",
            alignItems: "center",
            gap: 1,
            textDecoration: "none",
            color: isActive("/videos") ? "#fff" : "#9ea6ac",
            fontSize: "1.05rem",
            fontWeight: isActive("/videos") ? 600 : 400,
            "&:hover": { color: "#fff" },
          }}
        >
          <VideoLibraryIcon fontSize="small" />
          Vídeos
        </Typography>
      </Box>

      <Box flex={1} px={3} py={4} maxWidth="1200px" mx="auto" width="100%" sx={{ color: "#9ea6ac" }}>
        {children}
      </Box>
    </Box>
  );
}