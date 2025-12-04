import { Box } from '@mui/material';
import '../constants/metadata';
import './styles.css';

export default function RootLayout({
    children,
}: {
    children: React.ReactNode
}) {
    return (
        <html lang="en">
            <head>
                <link rel="preconnect" href="https://fonts.googleapis.com" />
                <link rel="preconnect" href="https://fonts.gstatic.com" />
                <link
                    rel="stylesheet"
                    href="https://fonts.googleapis.com/css2?family=Roboto:wght@300;400;500;700&display=swap"
                />
                <link rel="icon" href="/favicon.ico"/>
                <title>Clear Vision</title>
            </head>
            <body>
                <div id="root">
                    <Box
                        display="flex"
                        justifyContent="center"
                        alignItems="center"
                        minHeight="100vh"
                        sx={{
                            background:
                                "linear-gradient(135deg, #0d1117 0%, #0b1720 80%)",
                            color: "#e0e0e0",
                        }}
                    >
                        {children}
                    </Box>
                </div>
            </body>
        </html>
    );
}
