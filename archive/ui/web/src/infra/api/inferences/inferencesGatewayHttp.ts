import { env } from "@infra/env";

export default class InferencesGatewayHttp {
    private baseURL = `${env.NEXT_PUBLIC_API_URL}/clear-vision/v1`;

    async addInference(video_uid: string, target: string): Promise<any> {
        const url = `${this.baseURL}/inferences/?video_uid=${video_uid}&target=${encodeURIComponent(target)}`;

        const response = await fetch(url, {
            method: "POST"
        });

        if (!response.ok) {
            throw new Error(`Erro ao adicionar inferência: ${response.status}`);
        }

        return response.json();
    }
}
