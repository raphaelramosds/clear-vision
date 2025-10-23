import path from "path";
import type { NextConfig } from "next"

const nextConfig = {
  output: 'export',
  distDir: '../dist-ui',
  webpack(config) {
    config.resolve = config.resolve || {};
    config.resolve.alias = {
      ...(config.resolve.alias || {}),
      "@": path.resolve(__dirname, "src")
    };
    return config;
  }
}
 
export default nextConfig