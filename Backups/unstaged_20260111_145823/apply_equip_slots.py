"""
Apply EquipSlots tags to item assets.

This script applies equip slot tags either from extracted cache or from SubCategory mapping.
Run on SLFConversion project after migration.
"""

import unreal
import json
import os

# Mapping from SubCategory to EquipSlots
# Based on typical Souls-like equipment categories
SUBCATEGORY_TO_SLOTS = {
    # Weapons - Right Hand
    "Swords": ["SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 1",
               "SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 2",
               "SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 3"],
    "GreatSwords": ["SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 1",
                    "SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 2",
                    "SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 3"],
    "Axes": ["SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 1",
             "SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 2",
             "SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 3"],
    "GreatAxes": ["SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 1",
                  "SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 2",
                  "SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 3"],
    "Hammers": ["SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 1",
                "SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 2",
                "SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 3"],
    "Spears": ["SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 1",
               "SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 2",
               "SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 3"],
    "Daggers": ["SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 1",
                "SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 2",
                "SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 3"],
    "Katanas": ["SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 1",
                "SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 2",
                "SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 3"],
    "OneHanded": ["SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 1",
                  "SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 2",
                  "SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 3"],
    "TwoHanded": ["SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 1",
                  "SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 2",
                  "SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 3"],

    # Shields - Left Hand
    "Shields": ["SoulslikeFramework.Equipment.SlotType.Left Hand Weapon 1",
                "SoulslikeFramework.Equipment.SlotType.Left Hand Weapon 2",
                "SoulslikeFramework.Equipment.SlotType.Left Hand Weapon 3"],
    "GreatShields": ["SoulslikeFramework.Equipment.SlotType.Left Hand Weapon 1",
                     "SoulslikeFramework.Equipment.SlotType.Left Hand Weapon 2",
                     "SoulslikeFramework.Equipment.SlotType.Left Hand Weapon 3"],

    # Armor
    "Head": ["SoulslikeFramework.Equipment.SlotType.Head"],
    "Chest": ["SoulslikeFramework.Equipment.SlotType.Armor"],
    "Body": ["SoulslikeFramework.Equipment.SlotType.Armor"],
    "Arms": ["SoulslikeFramework.Equipment.SlotType.Gloves"],
    "Legs": ["SoulslikeFramework.Equipment.SlotType.Greaves"],

    # Accessories
    "Rings": ["SoulslikeFramework.Equipment.SlotType.Trinket 1",
              "SoulslikeFramework.Equipment.SlotType.Trinket 2"],
    "Talismans": ["SoulslikeFramework.Equipment.SlotType.Trinket 1",
                  "SoulslikeFramework.Equipment.SlotType.Trinket 2"],

    # Ammo
    "Arrows": ["SoulslikeFramework.Equipment.SlotType.Arrow"],
    "Bolts": ["SoulslikeFramework.Equipment.SlotType.Bullet"],
}

# Manual overrides for specific items (highest priority)
ITEM_OVERRIDES = {
    # Weapons
    "DA_Sword01": ["SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 1",
                   "SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 2",
                   "SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 3"],
    "DA_Sword02": ["SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 1",
                   "SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 2",
                   "SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 3"],
    "DA_PoisonSword": ["SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 1",
                       "SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 2",
                       "SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 3"],
    "DA_Katana": ["SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 1",
                  "SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 2",
                  "SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 3"],
    "DA_Greatsword": ["SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 1",
                      "SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 2",
                      "SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 3"],
    "DA_BossMace": ["SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 1",
                    "SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 2",
                    "SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 3"],

    # Shields
    "DA_Shield01": ["SoulslikeFramework.Equipment.SlotType.Left Hand Weapon 1",
                    "SoulslikeFramework.Equipment.SlotType.Left Hand Weapon 2",
                    "SoulslikeFramework.Equipment.SlotType.Left Hand Weapon 3"],

    # Armor
    "DA_ExampleArmor": ["SoulslikeFramework.Equipment.SlotType.Armor"],
    "DA_ExampleArmor02": ["SoulslikeFramework.Equipment.SlotType.Armor"],
    "DA_ExampleBracers": ["SoulslikeFramework.Equipment.SlotType.Gloves"],
    "DA_ExampleGreaves": ["SoulslikeFramework.Equipment.SlotType.Greaves"],
    "DA_ExampleHelmet": ["SoulslikeFramework.Equipment.SlotType.Head"],
    "DA_ExampleHat": ["SoulslikeFramework.Equipment.SlotType.Head"],

    # Accessories
    "DA_ExampleTalisman": ["SoulslikeFramework.Equipment.SlotType.Trinket 1",
                           "SoulslikeFramework.Equipment.SlotType.Trinket 2"],
}

def apply_equip_slots():
    """Apply EquipSlots to all item assets"""

    unreal.log_warning("=" * 80)
    unreal.log_warning("APPLYING EQUIP SLOTS")
    unreal.log_warning("=" * 80)

    # Try to load cached data first
    cache_path = "C:/scripts/SLFConversion/migration_cache/equip_slots.json"
    cached_data = {}
    if os.path.exists(cache_path):
        with open(cache_path, "r") as f:
            cached_data = json.load(f)
        unreal.log_warning(f"Loaded {len(cached_data)} items from cache")

    items_path = "/Game/SoulslikeFramework/Data/Items"
    assets = unreal.EditorAssetLibrary.list_assets(items_path, recursive=True, include_folder=False)

    success_count = 0
    skip_count = 0
    error_count = 0

    for asset_path in assets:
        asset_name = asset_path.split('/')[-1].split('.')[0]

        # Skip non-DA assets
        if not asset_name.startswith('DA_'):
            continue

        asset = unreal.load_asset(asset_path)
        if not asset:
            unreal.log_warning(f"Could not load: {asset_path}")
            error_count += 1
            continue

        unreal.log_warning(f"\nProcessing: {asset_name}")

        try:
            # Get ItemInformation struct
            item_info = asset.get_editor_property('item_information')
            if item_info is None:
                unreal.log_warning(f"  No item_information property")
                error_count += 1
                continue

            # Get EquipmentDetails struct
            equip_details = item_info.get_editor_property('equipment_details')
            if equip_details is None:
                unreal.log_warning(f"  No equipment_details property")
                error_count += 1
                continue

            # Determine which slots to apply (priority: override > cache > subcategory)
            slots_to_apply = []

            # 1. Check manual overrides first
            if asset_name in ITEM_OVERRIDES:
                slots_to_apply = ITEM_OVERRIDES[asset_name]
                unreal.log_warning(f"  Using manual override: {len(slots_to_apply)} slots")

            # 2. Check cached data
            elif asset_name in cached_data and cached_data[asset_name].get("EquipSlots"):
                slots_to_apply = cached_data[asset_name]["EquipSlots"]
                unreal.log_warning(f"  Using cached data: {len(slots_to_apply)} slots")

            # 3. Try SubCategory mapping
            else:
                # Get SubCategory from item
                try:
                    category_data = item_info.get_editor_property('category')
                    if category_data:
                        sub_cat = category_data.get_editor_property('sub_category')
                        if sub_cat:
                            sub_cat_str = str(sub_cat).split('.')[-1]  # Get enum value name
                            if sub_cat_str in SUBCATEGORY_TO_SLOTS:
                                slots_to_apply = SUBCATEGORY_TO_SLOTS[sub_cat_str]
                                unreal.log_warning(f"  Using SubCategory mapping ({sub_cat_str}): {len(slots_to_apply)} slots")
                except Exception as e:
                    unreal.log_warning(f"  Could not get SubCategory: {e}")

            if not slots_to_apply:
                unreal.log_warning(f"  No equip slots to apply - skipping")
                skip_count += 1
                continue

            # Create tag container with the slots
            equip_slots = equip_details.get_editor_property('equip_slots')

            # Get GameplayTagsManager to request tags
            tag_manager = unreal.GameplayTagsManager.get()

            for tag_str in slots_to_apply:
                try:
                    # Request tag by name using the manager
                    tag = tag_manager.request_gameplay_tag(unreal.Name(tag_str), False)
                    if tag.is_valid():
                        equip_slots.add_tag(tag)
                        unreal.log_warning(f"    Added: {tag_str}")
                    else:
                        unreal.log_warning(f"    Invalid tag (not registered): {tag_str}")
                except Exception as e:
                    unreal.log_warning(f"    Error adding tag {tag_str}: {e}")

            # Write back to struct
            equip_details.set_editor_property('equip_slots', equip_slots)
            item_info.set_editor_property('equipment_details', equip_details)
            asset.set_editor_property('item_information', item_info)

            # Save
            unreal.EditorAssetLibrary.save_loaded_asset(asset)
            unreal.log_warning(f"  SAVED")
            success_count += 1

        except Exception as e:
            unreal.log_error(f"Error processing {asset_name}: {e}")
            error_count += 1

    unreal.log_warning("\n" + "=" * 80)
    unreal.log_warning(f"COMPLETE: {success_count} success, {skip_count} skipped, {error_count} errors")
    unreal.log_warning("=" * 80)

if __name__ == "__main__":
    apply_equip_slots()
