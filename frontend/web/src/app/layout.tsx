import '../constants/metadata';

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
            </head>
            <body style={{
                margin: 0,  
                fontFamily: 'Roboto',
                fontWeight: 400
            }}>
                <div id="root">{children}</div>
            </body>
        </html>
    );
}