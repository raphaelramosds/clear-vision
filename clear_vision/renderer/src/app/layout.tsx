import { Metadata } from "next";

export const metadata: Metadata = {
  title: 'Clear Vision',
  description: 'Desktop solution to remove irrelevant frames from a video',
}

export default function RootLayout({
    children,
}: {
    children: React.ReactNode
}) {
    return (
        <html lang="en">
            <body>
                <div id="root">{children}</div>
            </body>
        </html>
  );
}