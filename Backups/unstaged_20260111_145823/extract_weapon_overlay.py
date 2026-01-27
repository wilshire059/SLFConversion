"""Extract WeaponOverlay tags from item data assets using export_text().
Run this on the bp_only project BEFORE restore to capture overlay data.
"""
import unreal
import json
import os
import re

CACHE_DIR = "C:/scripts/SLFConversion/migration_cache"
CACHE_FILE = os.path.join(CACHE_DIR, "weapon_overlay_data.json")

def log(msg):
    unreal.log_warning(str(msg))

def extract_weapon_overlay_from_text(export_text):
    """Extract WeaponOverlay tag from exported text.

    Looking for patterns like:
    WeaponOverlay=(TagName="SoulslikeFramework.Equipment.Weapons.Overlay.OneHanded")
    or with GUID suffix:
    WeaponOverlay_XX_GUID=(TagName="SoulslikeFramework.Equipment.Weapons.Overlay.TwoHanded",...)
    """
    # Pattern to find WeaponOverlay with TagName
    pattern = r'WeaponOverlay[^=]*=\(TagName="([^"]+)"'
    match = re.search(pattern, export_text)

    if match:
        tag_name = match.group(1)
        return tag_name

    return None

def extract_weapon_overlay_data():
    log("=" * 60)
    log("EXTRACTING WEAPON OVERLAY DATA FROM ITEMS")
    log("=" * 60)

    # Ensure cache directory exists
    os.makedirs(CACHE_DIR, exist_ok=True)

    overlay_data = {}
    extracted = 0
    skipped = 0
    items_path = "/Game/SoulslikeFramework/Data/Items"

    assets = unreal.EditorAssetLibrary.list_assets(items_path, recursive=True, include_folder=False)

    for asset_path in assets:
        asset_name = asset_path.split("/")[-1].split(".")[0]
        if not asset_name.startswith("DA_"):
            continue

        log(f"Processing: {asset_name}")

        asset = unreal.EditorAssetLibrary.load_asset(asset_path)
        if not asset:
            log(f"  [SKIP] Could not load")
            skipped += 1
            continue

        try:
            # Get ItemInformation struct
            item_info = asset.get_editor_property('ItemInformation')
            if not item_info:
                log(f"  [SKIP] No ItemInformation property")
                skipped += 1
                continue

            # Use export_text() on ItemInformation directly
            if not hasattr(item_info, 'export_text'):
                log(f"  [SKIP] No export_text() method on ItemInformation")
                skipped += 1
                continue

            export = item_info.export_text()

            # Parse for WeaponOverlay tag
            overlay_tag = extract_weapon_overlay_from_text(export)

            if overlay_tag:
                # Get the clean asset path (without .AssetName suffix)
                clean_path = asset_path.split(".")[0]
                overlay_data[asset_name] = {
                    "path": clean_path,
                    "weapon_overlay": overlay_tag
                }
                log(f"  [OK] {asset_name}: {overlay_tag}")
                extracted += 1
            else:
                log(f"  [INFO] No WeaponOverlay in {asset_name}")
                # Don't add to cache if no overlay - not all items have overlay tags

        except Exception as e:
            log(f"  [ERROR] {asset_name}: {e}")
            import traceback
            traceback.print_exc()
            skipped += 1

    # Save to cache file
    with open(CACHE_FILE, 'w') as f:
        json.dump(overlay_data, f, indent=2)

    log("")
    log("=" * 60)
    log(f"DONE: {extracted} extracted, {skipped} skipped")
    log(f"Cache saved to: {CACHE_FILE}")
    log("=" * 60)

    # Print what was found
    if overlay_data:
        log("\nExtracted WeaponOverlay tags:")
        for name, data in overlay_data.items():
            log(f"  {name}: {data['weapon_overlay']}")

    return overlay_data

if __name__ == "__main__":
    extract_weapon_overlay_data()
