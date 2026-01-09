"""Apply skeletal mesh info to armor items using C++ automation library."""
import unreal
import json
import os

CACHE_FILE = "C:/scripts/SLFConversion/migration_cache/armor_mesh_data.json"

def log(msg):
    unreal.log_warning(str(msg))

def apply_skeletal_mesh_info():
    log("=" * 60)
    log("APPLYING SKELETAL MESH INFO VIA C++ AUTOMATION")
    log("=" * 60)

    if not os.path.exists(CACHE_FILE):
        log(f"[ERROR] Cache file not found: {CACHE_FILE}")
        return

    with open(CACHE_FILE, 'r') as f:
        armor_data = json.load(f)

    succeeded = 0
    failed = 0
    skipped = 0

    for asset_name, data in armor_data.items():
        asset_path = data['path']
        skeletal_mesh_info = data.get('skeletal_mesh_info', {})

        if not skeletal_mesh_info:
            log(f"[SKIP] {asset_name} - no skeletal mesh info in cache")
            skipped += 1
            continue

        log(f"\n--- {asset_name} ---")
        log(f"  Path: {asset_path}")
        log(f"  Mesh entries: {len(skeletal_mesh_info)}")

        # Convert dict to parallel arrays for C++ function
        char_names = list(skeletal_mesh_info.keys())
        mesh_paths = list(skeletal_mesh_info.values())

        log(f"  Characters: {char_names}")

        # Call the C++ function via Python binding
        result = unreal.SLFAutomationLibrary.apply_skeletal_mesh_info_to_item(
            asset_path, char_names, mesh_paths
        )

        if result:
            log(f"  [OK] Applied {len(skeletal_mesh_info)} mesh entries")
            succeeded += 1
        else:
            log(f"  [ERROR] Failed to apply mesh info")
            failed += 1

    log("\n" + "=" * 60)
    log(f"DONE: {succeeded} succeeded, {failed} failed, {skipped} skipped")
    log("=" * 60)

if __name__ == "__main__":
    apply_skeletal_mesh_info()
