# apply_niagara_and_icons.py - Apply cached icon AND niagara data to migrated assets

import unreal
import json
import os

DATA_CACHE_DIR = "C:/scripts/SLFConversion/migration_cache"

def apply_item_data():
    """Apply IconSmall, IconLargeOptional, AND WorldNiagaraSystem to items"""
    unreal.log_warning("=" * 60)
    unreal.log_warning("APPLYING ITEM DATA (Icons + Niagara)")
    unreal.log_warning("=" * 60)

    cache_file = os.path.join(DATA_CACHE_DIR, "item_data.json")
    if not os.path.exists(cache_file):
        unreal.log_warning(f"  Cache file not found: {cache_file}")
        return

    with open(cache_file, 'r') as f:
        item_data = json.load(f)

    items_path = "/Game/SoulslikeFramework/Data/Items"
    assets = unreal.EditorAssetLibrary.list_assets(items_path, recursive=True, include_folder=False)

    icon_count = 0
    niagara_count = 0

    for asset_path in assets:
        asset_name = asset_path.split("/")[-1].split(".")[0]
        if asset_name not in item_data:
            continue

        data = item_data[asset_name]
        asset = unreal.EditorAssetLibrary.load_asset(asset_path)
        if not asset:
            continue

        try:
            # Get the item_information struct
            item_info = asset.get_editor_property('item_information')
            if item_info is None:
                unreal.log_warning(f"  [SKIP] {asset_name} - no item_information")
                continue

            modified = False
            applied = []

            # Apply IconSmall
            if 'IconSmall' in data:
                icon_path = data['IconSmall'].rsplit(".", 1)[0] if "." in data['IconSmall'] else data['IconSmall']
                texture = unreal.EditorAssetLibrary.load_asset(icon_path)
                if texture:
                    item_info.set_editor_property('icon_small', texture)
                    modified = True
                    applied.append('icon')

            # Apply IconLargeOptional
            if 'IconLargeOptional' in data:
                icon_path = data['IconLargeOptional'].rsplit(".", 1)[0] if "." in data['IconLargeOptional'] else data['IconLargeOptional']
                texture = unreal.EditorAssetLibrary.load_asset(icon_path)
                if texture:
                    item_info.set_editor_property('icon_large_optional', texture)
                    modified = True

            # Apply WorldNiagaraSystem - this goes on the ASSET, not the struct
            if 'WorldNiagaraSystem' in data:
                niagara_path = data['WorldNiagaraSystem'].rsplit(".", 1)[0] if "." in data['WorldNiagaraSystem'] else data['WorldNiagaraSystem']
                niagara = unreal.EditorAssetLibrary.load_asset(niagara_path)
                if niagara:
                    try:
                        asset.set_editor_property('world_niagara_system', niagara)
                        modified = True
                        applied.append('niagara')
                        niagara_count += 1
                    except Exception as e:
                        unreal.log_warning(f"    {asset_name}: Failed to set niagara: {e}")

            if modified:
                asset.set_editor_property('item_information', item_info)
                unreal.EditorAssetLibrary.save_asset(asset_path, only_if_is_dirty=False)
                if 'icon' in applied:
                    icon_count += 1
                unreal.log_warning(f"  [OK] {asset_name}: {applied}")

        except Exception as e:
            unreal.log_warning(f"  [ERROR] {asset_name}: {e}")

    unreal.log_warning(f"\nApplied icons to {icon_count} items")
    unreal.log_warning(f"Applied niagara to {niagara_count} items")

if __name__ == "__main__":
    apply_item_data()
