import { defineConfig } from "vite";
import React from "@vitejs/plugin-react";
import viteCompression from "vite-plugin-compression";

export default defineConfig(({ mode }) => {
  return {
    plugins: [
      React(),
      viteCompression({
        algorithm: "gzip",
        ext: ".gz",
        filter: () => true, // Compress all files
        deleteOriginFile: true, // Removes original files after compression
      }),
    ],
    server: {
      proxy:
        mode === "development"
          ? {
              "/api/": {
                target: "http://192.168.4.1/", // esp32 hotspot server ip
                changeOrigin: true,
                secure: false,
              },
            }
          : undefined,
    },
    build: {
      rollupOptions: {
        output: {
          // Disable Hash To Reduce File Length Due To ESP32 SPIFFS File Name Length Limitaion (path < 32 characters)
          assetFileNames: "assets/[name].[ext]",
          chunkFileNames: "chunks/[name].js",
          entryFileNames: "js/[name].js",
        },
      },
    },
  };
});
