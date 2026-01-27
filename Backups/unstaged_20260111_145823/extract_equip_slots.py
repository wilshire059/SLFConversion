"""
Extract EquipSlots data from backup item assets.

Run on bp_only project to extract the EquipSlots tags from items.
"""

import unreal
import json

def extract_equip_slots():
    """Extract EquipSlots from all item assets"""

    items_path = "/Game/SoulslikeFramework/Data/Items"

    # Get all assets in Items folder
    asset_registry = unreal.AssetRegistryHelpers.get_asset_registry()

    # Scan path
    assets = unreal.EditorAssetLibrary.list_assets(items_path, recursive=True, include_folder=False)

    unreal.log_warning(f"Found {len(assets)} assets in {items_path}")

    result = {}

    for asset_path in assets:
        # Skip non-DA assets
        if not asset_path.split('/')[-1].startswith('DA_'):
            continue

        asset = unreal.load_asset(asset_path)
        if not asset:
            unreal.log_warning(f"Could not load: {asset_path}")
            continue

        asset_name = asset.get_name()
        unreal.log_warning(f"Processing: {asset_name}")

        try:
            # Get ItemInformation struct
            item_info = asset.get_editor_property('item_information')
            if item_info is None:
                unreal.log_warning(f"  No item_information property")
                continue

            # Get EquipmentDetails struct
            equip_details = item_info.get_editor_property('equipment_details')
            if equip_details is None:
                unreal.log_warning(f"  No equipment_details property")
                continue

            # Get EquipSlots tag container
            equip_slots = equip_details.get_editor_property('equip_slots')
            if equip_slots is None:
                unreal.log_warning(f"  No equip_slots property")
                continue

            # Convert tag container to list of tag strings
            tags = []
            # FGameplayTagContainer has a method to get all tags
            tag_array = equip_slots.get_gameplay_tag_array()
            for tag in tag_array:
                tag_name = tag.get_tag_name()
                tags.append(str(tag_name))
                unreal.log_warning(f"  EquipSlot tag: {tag_name}")

            if tags:
                result[asset_name] = {"EquipSlots": tags}
                unreal.log_warning(f"  Found {len(tags)} equip slots")
            else:
                unreal.log_warning(f"  No equip slots defined")

        except Exception as e:
            unreal.log_error(f"Error processing {asset_name}: {e}")

    # Save to cache file
    cache_path = "C:/scripts/SLFConversion/migration_cache/equip_slots.json"
    with open(cache_path, "w") as f:
        json.dump(result, f, indent=2)

    unreal.log_warning(f"Saved {len(result)} items to {cache_path}")
    return result

if __name__ == "__main__":
    unreal.log_warning("=== Extracting EquipSlots Data ===")
    extract_equip_slots()
    unreal.log_warning("=== Extraction Complete ===")
