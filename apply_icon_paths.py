# apply_icon_paths.py
# Applies IconSmall and IconLargeOptional paths to item assets after reparenting
# Run in Unreal AFTER migration/reparenting
#
# Usage:
# 1. First run extract_icon_paths.py on backup content
# 2. Run migration (run_migration.py)
# 3. Run: UnrealEditor-Cmd.exe SLFConversion.uproject -run=pythonscript -script="apply_icon_paths.py"

import unreal
import json
import os

INPUT_FILE = "C:/scripts/SLFConversion/icon_paths.json"

def apply_icon_paths():
    """
    Apply IconSmall and IconLargeOptional paths to item assets.
    Reads from JSON file and sets the C++ properties on reparented assets.
    """
    unreal.log_warning("")
    unreal.log_warning("=" * 60)
    unreal.log_warning("APPLYING ICON PATHS TO ITEM ASSETS")
    unreal.log_warning("=" * 60)

    # Load icon data
    if not os.path.exists(INPUT_FILE):
        unreal.log_warning(f"[ERROR] Icon paths file not found: {INPUT_FILE}")
        unreal.log_warning("Run extract_icon_paths.py first on backup content!")
        return

    with open(INPUT_FILE, 'r') as f:
        icon_data = json.load(f)

    unreal.log_warning(f"Loaded {len(icon_data)} items with icon data")

    items_path = "/Game/SoulslikeFramework/Data/Items"
    assets = unreal.EditorAssetLibrary.list_assets(items_path, recursive=True, include_folder=False)

    modified_count = 0

    for asset_path in assets:
        asset_name = asset_path.split("/")[-1].split(".")[0]
        if not asset_name.startswith("DA_"):
            continue

        if asset_name not in icon_data:
            continue

        icons = icon_data[asset_name]
        unreal.log_warning(f"\nApplying icons to: {asset_name}")

        asset = unreal.EditorAssetLibrary.load_asset(asset_path)
        if not asset:
            unreal.log_warning(f"  [SKIP] Could not load")
            continue

        try:
            # Get ItemInformation struct
            item_info = asset.get_editor_property('item_information')
            if item_info is None:
                unreal.log_warning(f"  [SKIP] No item_information property (not reparented to C++?)")
                continue

            modified = False

            # Apply IconSmall
            if 'IconSmall' in icons:
                icon_path = icons['IconSmall']
                # Clean the path - get just the asset path without class type
                if "'" in icon_path:
                    icon_path = icon_path.split("'")[0]
                # Remove the .ClassName suffix if present
                if "." in icon_path:
                    parts = icon_path.rsplit(".", 1)
                    if parts[0].endswith("/" + parts[1]):
                        icon_path = parts[0]

                # Load the texture asset
                texture = unreal.load_asset(icon_path)
                if texture:
                    item_info.set_editor_property('icon_small', texture)
                    unreal.log_warning(f"  [OK] Set IconSmall: {texture.get_name()}")
                    modified = True
                else:
                    unreal.log_warning(f"  [WARN] Could not load texture: {icon_path}")

            # Apply IconLargeOptional
            if 'IconLargeOptional' in icons:
                icon_path = icons['IconLargeOptional']
                if "'" in icon_path:
                    icon_path = icon_path.split("'")[0]
                if "." in icon_path:
                    parts = icon_path.rsplit(".", 1)
                    if parts[0].endswith("/" + parts[1]):
                        icon_path = parts[0]

                texture = unreal.load_asset(icon_path)
                if texture:
                    item_info.set_editor_property('icon_large_optional', texture)
                    unreal.log_warning(f"  [OK] Set IconLargeOptional: {texture.get_name()}")
                    modified = True
                else:
                    unreal.log_warning(f"  [WARN] Could not load large texture: {icon_path}")

            if modified:
                # Set the modified struct back
                asset.set_editor_property('item_information', item_info)

                # Save the asset
                if unreal.EditorAssetLibrary.save_asset(asset_path, only_if_is_dirty=False):
                    unreal.log_warning(f"  [SAVED]")
                    modified_count += 1
                else:
                    unreal.log_warning(f"  [WARNING] Failed to save")

        except Exception as e:
            unreal.log_warning(f"  [ERROR] {e}")
            import traceback
            traceback.print_exc()

    unreal.log_warning("")
    unreal.log_warning("=" * 60)
    unreal.log_warning(f"COMPLETED: Modified {modified_count} items")
    unreal.log_warning("=" * 60)


if __name__ == "__main__":
    apply_icon_paths()
