# apply_niagara_paths.py
# Applies WorldNiagaraSystem paths from JSON to C++ properties on reparented items
# Run in Unreal AFTER reparenting PDA_Item to UPDA_Item

import unreal
import json
import os

INPUT_FILE = "C:/scripts/SLFConversion/niagara_paths.json"
OUTPUT_FILE = "C:/scripts/SLFConversion/check_output.txt"

def apply_niagara_paths():
    """
    Apply WorldNiagaraSystem paths from JSON to C++ properties.
    """
    lines = []
    lines.append("=" * 60)
    lines.append("APPLYING NIAGARA PATHS TO C++ PROPERTIES")
    lines.append("=" * 60)

    # Load the extracted data
    if not os.path.exists(INPUT_FILE):
        lines.append(f"[ERROR] Input file not found: {INPUT_FILE}")
        with open(OUTPUT_FILE, 'w') as f:
            f.write('\n'.join(lines))
        return

    with open(INPUT_FILE, 'r') as f:
        niagara_data = json.load(f)

    lines.append(f"\nLoaded {len(niagara_data)} Niagara paths from JSON")

    if not niagara_data:
        lines.append("[WARNING] No Niagara data to apply")
        with open(OUTPUT_FILE, 'w') as f:
            f.write('\n'.join(lines))
        return

    items_path = "/Game/SoulslikeFramework/Data/Items"
    assets = unreal.EditorAssetLibrary.list_assets(items_path, recursive=True, include_folder=False)

    applied = 0
    failed = 0

    for asset_path in assets:
        asset_name = asset_path.split("/")[-1].split(".")[0]

        if asset_name not in niagara_data:
            continue

        niagara_path = niagara_data[asset_name]

        # Load the asset
        asset = unreal.EditorAssetLibrary.load_asset(asset_path)
        if not asset:
            lines.append(f"  {asset_name}: SKIP - Could not load")
            failed += 1
            continue

        try:
            # Load the actual Niagara system asset
            niagara_asset = unreal.EditorAssetLibrary.load_asset(niagara_path)
            if not niagara_asset:
                lines.append(f"  {asset_name}: SKIP - Niagara not found: {niagara_path}")
                failed += 1
                continue

            # Set using set_editor_property with snake_case
            asset.set_editor_property('world_niagara_system', niagara_asset)

            # Save the asset
            save_result = unreal.EditorAssetLibrary.save_asset(asset_path, only_if_is_dirty=False)

            if save_result:
                ns_name = niagara_path.split('/')[-1].split('.')[0]
                lines.append(f"  {asset_name}: OK - {ns_name}")
                applied += 1
            else:
                lines.append(f"  {asset_name}: SAVE FAILED")
                failed += 1

        except Exception as e:
            lines.append(f"  {asset_name}: ERROR - {e}")
            failed += 1

    lines.append("")
    lines.append("=" * 60)
    lines.append(f"COMPLETE: Applied {applied}, Failed {failed}")
    lines.append("=" * 60)

    # Verification
    lines.append("\nVerification - DA_Apple:")
    try:
        apple = unreal.EditorAssetLibrary.load_asset("/Game/SoulslikeFramework/Data/Items/DA_Apple")
        if apple:
            niagara = apple.get_editor_property('world_niagara_system')
            lines.append(f"  world_niagara_system: {niagara}")
            if niagara:
                lines.append(f"  Niagara name: {niagara.get_name()}")
    except Exception as e:
        lines.append(f"  Verification error: {e}")

    with open(OUTPUT_FILE, 'w') as f:
        f.write('\n'.join(lines))

    print(f"Output written to {OUTPUT_FILE}")


if __name__ == "__main__":
    apply_niagara_paths()
