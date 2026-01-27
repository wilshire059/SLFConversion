"""Apply WeaponOverlay tags to item data assets from cached data.
Run this AFTER migration to restore overlay data.
"""
import unreal
import json
import os

CACHE_FILE = "C:/scripts/SLFConversion/migration_cache/weapon_overlay_data.json"

def log(msg):
    unreal.log_warning(str(msg))

def apply_weapon_overlay_data():
    log("=" * 60)
    log("APPLYING WEAPON OVERLAY DATA TO ITEMS")
    log("=" * 60)

    if not os.path.exists(CACHE_FILE):
        log(f"[ERROR] Cache file not found: {CACHE_FILE}")
        log("Run extract_weapon_overlay.py on bp_only project first!")
        return False

    with open(CACHE_FILE, 'r') as f:
        overlay_data = json.load(f)

    succeeded = 0
    failed = 0
    skipped = 0

    for asset_name, data in overlay_data.items():
        asset_path = data['path']
        overlay_tag = data.get('weapon_overlay', '')

        if not overlay_tag:
            log(f"  [SKIP] {asset_name}: no overlay tag in cache")
            skipped += 1
            continue

        log(f"  Applying to {asset_name}: {overlay_tag}")

        # Use the C++ automation function
        try:
            result = unreal.SLFAutomationLibrary.apply_weapon_overlay_tag(
                asset_path, overlay_tag
            )

            if result:
                log(f"  [OK] {asset_name}")
                succeeded += 1
            else:
                log(f"  [FAILED] {asset_name}")
                failed += 1

        except Exception as e:
            log(f"  [ERROR] {asset_name}: {e}")
            failed += 1

    log("")
    log("=" * 60)
    log(f"DONE: {succeeded} succeeded, {failed} failed, {skipped} skipped")
    log("=" * 60)

    return failed == 0

if __name__ == "__main__":
    apply_weapon_overlay_data()
