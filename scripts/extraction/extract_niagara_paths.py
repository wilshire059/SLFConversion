# extract_niagara_paths.py
# Extracts WorldNiagaraSystem paths from item assets using export_text()
# Run in Unreal after restoring backup items (before reparenting)

import unreal
import json
import re

OUTPUT_FILE = "C:/scripts/SLFConversion/niagara_paths.json"

def extract_niagara_paths():
    """
    Extract WorldNiagaraSystem paths from all DA_* item assets.
    Uses export_text() to get struct contents and parses for Niagara paths.
    """
    print("")
    print("=" * 60)
    print("EXTRACTING NIAGARA PATHS FROM ITEM ASSETS")
    print("=" * 60)

    niagara_data = {}
    items_path = "/Game/SoulslikeFramework/Data/Items"

    assets = unreal.EditorAssetLibrary.list_assets(items_path, recursive=True, include_folder=False)

    for asset_path in assets:
        asset_name = asset_path.split("/")[-1].split(".")[0]
        if not asset_name.startswith("DA_"):
            continue

        print(f"\nProcessing: {asset_name}")

        asset = unreal.EditorAssetLibrary.load_asset(asset_path)
        if not asset:
            print(f"  [SKIP] Could not load")
            continue

        asset_class = asset.get_class()
        print(f"  Class: {asset_class.get_name()}")

        try:
            # Get ItemInformation struct
            item_info = asset.get_editor_property('ItemInformation')
            if not item_info:
                print(f"  [SKIP] No ItemInformation property")
                continue

            # Use export_text() to get struct contents
            if not hasattr(item_info, 'export_text'):
                print(f"  [SKIP] No export_text() method")
                continue

            export = item_info.export_text()

            # Parse for WorldNiagaraSystem
            # Format: WorldNiagaraSystem_XX_GUID="/Game/Path/To/NiagaraSystem.NiagaraSystem"
            match = re.search(r'WorldNiagaraSystem[^=]*="([^"]+)"', export)

            if match:
                niagara_path = match.group(1)
                if niagara_path and niagara_path != "None" and niagara_path != "":
                    niagara_data[asset_name] = niagara_path
                    print(f"  [OK] Found: {niagara_path}")
                else:
                    print(f"  [INFO] WorldNiagaraSystem is empty")
            else:
                print(f"  [INFO] No WorldNiagaraSystem in export_text()")

        except Exception as e:
            print(f"  [ERROR] {e}")
            import traceback
            traceback.print_exc()

    # Save to JSON file
    print("")
    print("=" * 60)
    print(f"SAVING TO {OUTPUT_FILE}")
    print("=" * 60)

    with open(OUTPUT_FILE, 'w') as f:
        json.dump(niagara_data, f, indent=2)

    print(f"\nExtracted {len(niagara_data)} Niagara paths:")
    for name, path in niagara_data.items():
        print(f"  {name}: {path}")

    return niagara_data


if __name__ == "__main__":
    extract_niagara_paths()
