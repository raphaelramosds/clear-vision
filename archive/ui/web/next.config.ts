import path from "path";

const nextConfig = {
  output: 'standalone',
  distDir: '../dist-ui',
  webpack(config) {
    config.resolve = config.resolve || {};
    config.resolve.alias = {
      ...(config.resolve.alias || {}),
      "@": path.resolve(__dirname, "src"),
      "@infra": path.resolve(__dirname, "src/infra"),
    };
    return config;
  }
}
 
export default nextConfig