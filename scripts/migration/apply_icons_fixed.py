import unreal
import json
import os

CACHE_DIR = "C:/scripts/SLFConversion/migration_cache"

def apply_icons():
    unreal.log_warning("=" * 60)
    unreal.log_warning("APPLYING ICONS (Fixed)")
    unreal.log_warning("=" * 60)

    cache_file = os.path.join(CACHE_DIR, "item_data.json")
    if not os.path.exists(cache_file):
        unreal.log_warning("  Cache not found!")
        return

    with open(cache_file, 'r') as f:
        item_data = json.load(f)

    items_path = "/Game/SoulslikeFramework/Data/Items"
    assets = unreal.EditorAssetLibrary.list_assets(items_path, recursive=True, include_folder=False)

    count = 0
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
                unreal.log_warning("  [SKIP] " + asset_name + " - no item_information")
                continue

            modified = False

            # Apply IconSmall
            if 'IconSmall' in data:
                icon_path = data['IconSmall']
                # Remove class suffix if present
                if "." in icon_path:
                    icon_path = icon_path.rsplit(".", 1)[0]
                texture = unreal.EditorAssetLibrary.load_asset(icon_path)
                if texture:
                    item_info.set_editor_property('icon_small', texture)
                    modified = True
                    unreal.log_warning("  [OK] " + asset_name + " icon_small: " + texture.get_name())
                else:
                    unreal.log_warning("  [WARN] " + asset_name + " could not load: " + icon_path)

            # Apply IconLargeOptional
            if 'IconLargeOptional' in data:
                icon_path = data['IconLargeOptional']
                if "." in icon_path:
                    icon_path = icon_path.rsplit(".", 1)[0]
                texture = unreal.EditorAssetLibrary.load_asset(icon_path)
                if texture:
                    item_info.set_editor_property('icon_large_optional', texture)
                    modified = True

            if modified:
                # Write the struct back to the asset
                asset.set_editor_property('item_information', item_info)
                unreal.EditorAssetLibrary.save_asset(asset_path, only_if_is_dirty=False)
                count += 1

        except Exception as e:
            unreal.log_warning("  [ERROR] " + asset_name + ": " + str(e))
            import traceback
            traceback.print_exc()

    unreal.log_warning("Applied icons to " + str(count) + " items")

if __name__ == "__main__":
    apply_icons()
