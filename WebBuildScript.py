Import("env")
import os


def before_build_spiffs(source, target, env):
    print("Building React App...")
    if env.Execute("cd webapp && npm run build -- --logLevel warn") != 0:
        raise RuntimeError("React App build failed")

    print("React App built!")
    print("Removing old SPIFFS image...")
    if env.Execute("node file_operations.js remove data/dist") != 0:
        raise RuntimeError("Failed to remove old SPIFFS assets")

    print("Copying React App to SPIFFS...")
    if env.Execute("node file_operations.js copy webapp/dist data/dist") != 0:
        raise RuntimeError("Failed to copy React App assets")


env.AddPreAction("$BUILD_DIR/spiffs.bin", before_build_spiffs)
