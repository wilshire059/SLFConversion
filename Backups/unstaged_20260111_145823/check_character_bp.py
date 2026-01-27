# check_character_bp.py
# Check the status of B_Soulslike_Character Blueprint

import unreal
import os

def log(msg):
    unreal.log_warning(f"[CHECK] {msg}")

def main():
    log("=" * 70)
    log("B_SOULSLIKE_CHARACTER CHECK")
    log("=" * 70)

    # Try different paths
    paths_to_try = [
        "/Game/SoulslikeFramework/Blueprints/Character/B_Soulslike_Character",
        "/Game/SoulslikeFramework/Blueprints/Character/B_Soulslike_Character.B_Soulslike_Character",
        "/Game/SoulslikeFramework/Demo/Blueprints/B_Soulslike_Character",
    ]

    for path in paths_to_try:
        log(f"Trying: {path}")
        try:
            asset = unreal.load_asset(path)
            if asset:
                log(f"  SUCCESS: Loaded {asset}")
                parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(asset)
                log(f"  Parent: {parent}")
                errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(asset)
                log(f"  Compile Status:")
                for line in errors.split('\n')[:10]:
                    if line.strip():
                        log(f"    {line}")
            else:
                log("  FAILED: Asset is None")
        except Exception as e:
            log(f"  ERROR: {e}")

    # Check if the file exists on disk
    log("")
    log("Checking file on disk:")
    file_path = "C:/scripts/SLFConversion/Content/SoulslikeFramework/Blueprints/Character/B_Soulslike_Character.uasset"
    if os.path.exists(file_path):
        log(f"  File EXISTS: {file_path}")
        log(f"  Size: {os.path.getsize(file_path)} bytes")
    else:
        log(f"  File NOT FOUND: {file_path}")
        # List what's in the directory
        dir_path = "C:/scripts/SLFConversion/Content/SoulslikeFramework/Blueprints/Character"
        if os.path.exists(dir_path):
            log(f"  Contents of {dir_path}:")
            for f in os.listdir(dir_path):
                log(f"    {f}")

    # Check EditorAssetLibrary
    log("")
    log("Using EditorAssetLibrary:")
    try:
        exists = unreal.EditorAssetLibrary.does_asset_exist("/Game/SoulslikeFramework/Blueprints/Character/B_Soulslike_Character")
        log(f"  Asset exists: {exists}")
    except Exception as e:
        log(f"  ERROR: {e}")

    log("")
    log("=" * 70)

if __name__ == "__main__":
    main()
