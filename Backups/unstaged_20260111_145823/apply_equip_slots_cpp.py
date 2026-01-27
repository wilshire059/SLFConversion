"""Apply equip slots to armor items using C++ automation library."""
import unreal
import json
import os

CACHE_FILE = "C:/scripts/SLFConversion/migration_cache/armor_mesh_data.json"

def log(msg):
    unreal.log_warning(str(msg))

def apply_equip_slots():
    log("=" * 60)
    log("APPLYING EQUIP SLOTS VIA C++ AUTOMATION")
    log("=" * 60)

    if not os.path.exists(CACHE_FILE):
        log(f"[ERROR] Cache file not found: {CACHE_FILE}")
        return

    with open(CACHE_FILE, 'r') as f:
        armor_data = json.load(f)

    succeeded = 0
    failed = 0

    for asset_name, data in armor_data.items():
        asset_path = data['path']
        equip_slots = data.get('equip_slots', [])

        if not equip_slots:
            log(f"[SKIP] {asset_name} - no equip slots in cache")
            continue

        log(f"\n--- {asset_name} ---")
        log(f"  Path: {asset_path}")
        log(f"  Slots: {equip_slots}")

        # Call the C++ function via Python binding
        result = unreal.SLFAutomationLibrary.apply_equip_slots_to_item(asset_path, equip_slots)

        if result:
            log(f"  [OK] Applied {len(equip_slots)} slot tags")
            succeeded += 1
        else:
            log(f"  [ERROR] Failed to apply slot tags")
            failed += 1

    log("\n" + "=" * 60)
    log(f"DONE: {succeeded} succeeded, {failed} failed")
    log("=" * 60)

if __name__ == "__main__":
    apply_equip_slots()
