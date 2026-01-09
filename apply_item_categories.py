"""
Apply item categories and usable flags to migrated item data assets.

Category enum values (ESLFItemCategory):
  None = 0
  Tools = 1
  Crafting = 2
  Bolstering = 3
  KeyItems = 4
  Abilities = 5
  Weapons = 6
  Shields = 7
  Armor = 8

Run on SLFConversion AFTER C++ migration:
UnrealEditor-Cmd.exe "C:/scripts/SLFConversion/SLFConversion.uproject" -run=pythonscript -script="C:/scripts/SLFConversion/apply_item_categories.py"
"""

import unreal

# Get the enum class - ESLFItemCategory is exposed as SLFItemCategory in Python
SLFItemCategory = unreal.SLFItemCategory

# Item category and usable mappings using actual enum values
ITEM_DATA = {
    # Weapons
    "DA_Sword01": {"category": SLFItemCategory.WEAPONS, "usable": False},
    "DA_Sword02": {"category": SLFItemCategory.WEAPONS, "usable": False},
    "DA_PoisonSword": {"category": SLFItemCategory.WEAPONS, "usable": False},
    "DA_Greatsword": {"category": SLFItemCategory.WEAPONS, "usable": False},
    "DA_Katana": {"category": SLFItemCategory.WEAPONS, "usable": False},
    "DA_BossMace": {"category": SLFItemCategory.WEAPONS, "usable": False},

    # Shields
    "DA_Shield01": {"category": SLFItemCategory.SHIELDS, "usable": False},

    # Armor
    "DA_ExampleArmor": {"category": SLFItemCategory.ARMOR, "usable": False},
    "DA_ExampleArmor02": {"category": SLFItemCategory.ARMOR, "usable": False},
    "DA_ExampleBracers": {"category": SLFItemCategory.ARMOR, "usable": False},
    "DA_ExampleGreaves": {"category": SLFItemCategory.ARMOR, "usable": False},
    "DA_ExampleHelmet": {"category": SLFItemCategory.ARMOR, "usable": False},
    "DA_ExampleHat": {"category": SLFItemCategory.ARMOR, "usable": False},

    # Tools - Usable items that trigger actions
    "DA_Lantern": {"category": SLFItemCategory.TOOLS, "usable": True, "action_tag": "Action.UseEquippedTool"},
    "DA_ThrowingKnife": {"category": SLFItemCategory.TOOLS, "usable": True, "action_tag": "Action.Projectile"},
    "DA_HealthFlask": {"category": SLFItemCategory.TOOLS, "usable": True, "action_tag": "Action.DrinkFlask.HP"},

    # Bolstering
    "DA_ExampleTalisman": {"category": SLFItemCategory.BOLSTERING, "usable": False},

    # Crafting
    "DA_Apple": {"category": SLFItemCategory.CRAFTING, "usable": True},  # Food item - consumable
    "DA_CrimsonEverbloom": {"category": SLFItemCategory.CRAFTING, "usable": False},  # Crafting material
    "DA_Cube": {"category": SLFItemCategory.CRAFTING, "usable": False},  # Crafting material

    # Key Items
    "DA_PrisonKey": {"category": SLFItemCategory.KEY_ITEMS, "usable": False},
}

ITEMS_PATH = "/Game/SoulslikeFramework/Data/Items/"
OUTPUT_FILE = "C:/scripts/SLFConversion/category_migration_output.txt"
OUTPUT_LINES = []

# Category names for logging
def get_category_name(cat_enum):
    """Get display name from category enum value."""
    if hasattr(cat_enum, 'name'):
        return cat_enum.name
    return str(cat_enum)

def log(msg):
    """Log to both Unreal and output file."""
    OUTPUT_LINES.append(str(msg))
    unreal.log_warning(str(msg))

def apply_categories():
    """Apply category and usable data to all item assets."""

    log("=" * 60)
    log("APPLYING ITEM CATEGORIES AND USABLE FLAGS")
    log("=" * 60)
    log(f"Processing {len(ITEM_DATA)} items...")
    log("")

    success_count = 0
    error_count = 0

    for item_name, data in ITEM_DATA.items():
        asset_path = f"{ITEMS_PATH}{item_name}.{item_name}"

        # Load the asset
        asset = unreal.EditorAssetLibrary.load_asset(asset_path)
        if not asset:
            log(f"  ERROR: Could not load {item_name}")
            error_count += 1
            continue

        try:
            # Get current ItemInformation struct
            item_info = asset.get_editor_property('item_information')
            if item_info is None:
                log(f"  ERROR: {item_name} - No item_information property")
                error_count += 1
                continue

            # Get current Category struct from ItemInformation
            category_data = item_info.get_editor_property('category')
            if category_data is None:
                log(f"  ERROR: {item_name} - No category property")
                error_count += 1
                continue

            # Get current values for logging
            old_category = category_data.get_editor_property('category')
            old_usable = item_info.get_editor_property('usable')

            # Set the category enum value
            new_category = data["category"]
            category_data.set_editor_property('category', new_category)

            # Update ItemInformation with new category
            item_info.set_editor_property('category', category_data)

            # Set usable flag
            new_usable = data.get("usable", False)
            item_info.set_editor_property('usable', new_usable)

            # Set action tag if specified
            if "action_tag" in data:
                # Create a gameplay tag from string
                try:
                    tag_string = data["action_tag"]
                    # Use GameplayTagsManager to request tag
                    tag = unreal.GameplayTagsManager.request_gameplay_tag(tag_string, False)
                    if tag.is_valid():
                        item_info.set_editor_property('action_to_trigger', tag)
                except Exception as tag_error:
                    log(f"    Warning: Could not set action_tag for {item_name}: {tag_error}")

            # Update the asset's ItemInformation
            asset.set_editor_property('item_information', item_info)

            # Save the asset
            unreal.EditorAssetLibrary.save_loaded_asset(asset)

            category_name = get_category_name(new_category)
            usable_str = "Usable" if new_usable else "Not Usable"
            log(f"  OK: {item_name}")
            log(f"      Category: {get_category_name(old_category)} -> {category_name}")
            log(f"      Usable: {old_usable} -> {new_usable} ({usable_str})")
            success_count += 1

        except Exception as e:
            log(f"  ERROR: {item_name} - {e}")
            import traceback
            log(traceback.format_exc())
            error_count += 1

    log(f"")
    log("=" * 60)
    log(f"Results: {success_count} succeeded, {error_count} failed")
    log("=" * 60)

    return success_count, error_count

def list_current_values():
    """List current category and usable values for all items."""

    log("=" * 60)
    log("CURRENT ITEM CATEGORY VALUES")
    log("=" * 60)

    item_assets = unreal.EditorAssetLibrary.list_assets(ITEMS_PATH, recursive=True, include_folder=False)

    for asset_path in item_assets:
        if '_C' in asset_path:
            continue

        asset_name = asset_path.split('/')[-1].split('.')[0]
        asset = unreal.EditorAssetLibrary.load_asset(asset_path)

        if not asset:
            log(f"  {asset_name}: Could not load")
            continue

        try:
            item_info = asset.get_editor_property('item_information')
            if item_info:
                category_data = item_info.get_editor_property('category')
                category_value = category_data.get_editor_property('category') if category_data else "N/A"
                usable = item_info.get_editor_property('usable')
                action = item_info.get_editor_property('action_to_trigger')
                action_valid = action.is_valid() if action else False

                category_name = get_category_name(category_value)
                log(f"  {asset_name}: Category={category_name}, bUsable={usable}, ActionValid={action_valid}")
            else:
                log(f"  {asset_name}: No item_information")
        except Exception as e:
            log(f"  {asset_name}: Error - {e}")

    log("=" * 60)

if __name__ == "__main__":
    # First list current values
    log("")
    log("=== BEFORE MIGRATION ===")
    log("")
    list_current_values()

    # Then apply the changes
    log("")
    log("=== APPLYING CHANGES ===")
    log("")
    success, errors = apply_categories()

    # Show results after
    log("")
    log("=== AFTER MIGRATION ===")
    log("")
    list_current_values()

    # Write output to file
    with open(OUTPUT_FILE, 'w') as f:
        f.write('\n'.join(OUTPUT_LINES))
    unreal.log_warning(f"Output written to {OUTPUT_FILE}")
