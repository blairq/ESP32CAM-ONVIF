Import("env")
import os
import subprocess

def generate_config(*args, **kwargs):
    print("Running kconfiglib pre-build script...")
    env_dir = os.environ.get("VIRTUAL_ENV", ".venv")
    python_exe = os.path.join(env_dir, "bin", "python")
    alldefconfig_exe = os.path.join(env_dir, "bin", "alldefconfig")

    # Run alldefconfig if .config doesn't exist to generate defaults
    if not os.path.exists(".config"):
        print(".config not found. Generating default configuration.")
        subprocess.run([alldefconfig_exe], check=True)

    print("Generating ESP32CAM-ONVIF/config.h from .config")
    # Make sure we only generate config.h if .config exists
    if os.path.exists(".config"):
        subprocess.run([python_exe, "gen_config.py", ".config", "ESP32CAM-ONVIF/config.h"], check=True)
    else:
        print("Error: .config still not found after running alldefconfig")

env.AddPreAction("buildprog", generate_config)
