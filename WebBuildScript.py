Import("env")
import os

def before_build_spiffs(source, target, env):
    print("Building React App...")
    env.Execute("cd intelligarden-web-app && npm run build")
    print("React App built!")
    print("Removing old SPIFFS image...")
    env.Execute("node file_operations.js remove data/dist")
    print("Copying React App to SPIFFS...")
    env.Execute("node file_operations.js copy intelligarden-web-app/dist data/dist")

env.AddPreAction("$BUILD_DIR/spiffs.bin", before_build_spiffs)
