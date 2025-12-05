import { env } from "@infra/env";

export default class VideosGatewayHttp {
    private baseURL = `${env.NEXT_PUBLIC_API_URL}/clear-vision/v1`;

    async uploadVideo(file: File): Promise<any> {
        const formData = new FormData();
        formData.append("video", file);

        const response = await fetch(`${this.baseURL}/videos/upload/`, {
            method: "POST",
            body: formData
        });

        if (!response.ok) {
            throw new Error(`Erro ao enviar vídeo: ${response.status}`);
        }

        return response.json();
    }

    async getVideoInferences(video_uid: string): Promise<any> {
        const response = await fetch(`${this.baseURL}/videos/${video_uid}/inferences`, {
            method: "GET",
            cache: "no-store"
        });

        if (!response.ok) {
            throw new Error(`Erro ao obter inferências: ${response.status}`);
        }

        return response.json();
    }

    async getVideos(): Promise<any> {
        const response = await fetch(`${this.baseURL}/videos`, { cache: "no-store" });
        return response.json();
    }
}
