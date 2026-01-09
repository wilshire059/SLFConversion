# apply_item_icons.py - Apply cached icon data to migrated assets

import unreal
import json
import os

DATA_CACHE_DIR = "C:/scripts/SLFConversion/migration_cache"

def apply_item_data():
    """Apply IconSmall, IconLargeOptional to reparented PDA_Item assets"""
    unreal.log_warning("=" * 60)
    unreal.log_warning("APPLYING ITEM ICON DATA")
    unreal.log_warning("=" * 60)

    cache_file = os.path.join(DATA_CACHE_DIR, "item_data.json")
    if not os.path.exists(cache_file):
        unreal.log_warning(f"  Cache file not found: {cache_file}")
        return

    with open(cache_file, 'r') as f:
        item_data = json.load(f)

    items_path = "/Game/SoulslikeFramework/Data/Items"
    assets = unreal.EditorAssetLibrary.list_assets(items_path, recursive=True, include_folder=False)

    modified_count = 0

    for asset_path in assets:
        asset_name = asset_path.split("/")[-1].split(".")[0]
        if asset_name not in item_data:
            continue

        data = item_data[asset_name]
        asset = unreal.EditorAssetLibrary.load_asset(asset_path)
        if not asset:
            unreal.log_warning(f"  [SKIP] {asset_name} - could not load")
            continue

        try:
            # Get the C++ struct property
            item_info = asset.get_editor_property('item_information')
            if item_info is None:
                unreal.log_warning(f"  [SKIP] {asset_name} - no item_information property")
                continue

            modified = False

            # Apply IconSmall
            if 'IconSmall' in data:
                icon_path = data['IconSmall']
                # Clean the path - remove .ClassName suffix
                if "." in icon_path:
                    base_path = icon_path.rsplit(".", 1)[0]
                else:
                    base_path = icon_path

                texture = unreal.EditorAssetLibrary.load_asset(base_path)
                if texture:
                    item_info.set_editor_property('icon_small', texture)
                    modified = True
                else:
                    unreal.log_warning(f"    Failed to load texture: {base_path}")

            # Apply IconLargeOptional
            if 'IconLargeOptional' in data:
                icon_path = data['IconLargeOptional']
                if "." in icon_path:
                    base_path = icon_path.rsplit(".", 1)[0]
                else:
                    base_path = icon_path

                texture = unreal.EditorAssetLibrary.load_asset(base_path)
                if texture:
                    item_info.set_editor_property('icon_large_optional', texture)
                    modified = True

            if modified:
                # Save the struct back to the asset
                asset.set_editor_property('item_information', item_info)
                unreal.EditorAssetLibrary.save_asset(asset_path, only_if_is_dirty=False)
                modified_count += 1
                unreal.log_warning(f"  [OK] {asset_name}")

        except Exception as e:
            unreal.log_warning(f"  [ERROR] {asset_name}: {e}")
            import traceback
            traceback.print_exc()

    unreal.log_warning(f"Applied icons to {modified_count} items")

if __name__ == "__main__":
    apply_item_data()
