import z from "zod";

const envSchema = z.object({
    NEXT_PUBLIC_API_URL: z.string().url()
});

const parsed = envSchema.safeParse({
    NEXT_PUBLIC_API_URL: process.env.NEXT_PUBLIC_API_URL
});

export const env = parsed.success ? parsed.data : (process.env as any);