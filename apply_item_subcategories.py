"""
Apply item subcategories to migrated item data assets.

SubCategory enum values (ESLFItemSubCategory):
  Flasks = 0
  Consumables = 1
  Projectiles = 2
  Enchants = 3
  Miscellaneous = 4
  Runes = 5
  Head = 6
  Chest = 7
  Arms = 8
  Legs = 9
  Talismans = 10
  InfoItems = 11
  Sword = 12
  Katana = 13
  Axe = 14
  Mace = 15
  Staff = 16

Run on SLFConversion AFTER C++ migration:
UnrealEditor-Cmd.exe "C:/scripts/SLFConversion/SLFConversion.uproject" -run=pythonscript -script="C:/scripts/SLFConversion/apply_item_subcategories.py"
"""

import unreal

# Get the enum class - ESLFItemSubCategory is exposed as SLFItemSubCategory in Python
SLFItemSubCategory = unreal.SLFItemSubCategory

# Item subcategory mappings
ITEM_SUBCATEGORY_DATA = {
    # Armor - Head pieces
    "DA_ExampleHelmet": SLFItemSubCategory.HEAD,
    "DA_ExampleHat": SLFItemSubCategory.HEAD,

    # Armor - Chest pieces
    "DA_ExampleArmor": SLFItemSubCategory.CHEST,
    "DA_ExampleArmor02": SLFItemSubCategory.CHEST,

    # Armor - Arms/Gloves pieces
    "DA_ExampleBracers": SLFItemSubCategory.ARMS,

    # Armor - Legs/Greaves pieces
    "DA_ExampleGreaves": SLFItemSubCategory.LEGS,

    # Weapons - specific subtypes
    "DA_Sword01": SLFItemSubCategory.SWORD,
    "DA_Sword02": SLFItemSubCategory.SWORD,
    "DA_PoisonSword": SLFItemSubCategory.SWORD,
    "DA_Greatsword": SLFItemSubCategory.SWORD,
    "DA_Katana": SLFItemSubCategory.KATANA,
    "DA_BossMace": SLFItemSubCategory.MACE,

    # Talismans
    "DA_ExampleTalisman": SLFItemSubCategory.TALISMANS,

    # Tools - Consumables/Flasks
    "DA_HealthFlask": SLFItemSubCategory.FLASKS,
    "DA_ThrowingKnife": SLFItemSubCategory.PROJECTILES,

    # Crafting
    "DA_Apple": SLFItemSubCategory.CONSUMABLES,
}

ITEMS_PATH = "/Game/SoulslikeFramework/Data/Items/"
OUTPUT_FILE = "C:/scripts/SLFConversion/subcategory_migration_output.txt"
OUTPUT_LINES = []

def get_subcategory_name(subcat_enum):
    """Get display name from subcategory enum value."""
    if hasattr(subcat_enum, 'name'):
        return subcat_enum.name
    return str(subcat_enum)

def log(msg):
    """Log to both Unreal and output file."""
    OUTPUT_LINES.append(str(msg))
    unreal.log_warning(str(msg))

def apply_subcategories():
    """Apply subcategory data to all item assets."""

    log("=" * 60)
    log("APPLYING ITEM SUBCATEGORIES")
    log("=" * 60)
    log(f"Processing {len(ITEM_SUBCATEGORY_DATA)} items...")
    log("")

    success_count = 0
    error_count = 0

    for item_name, new_subcategory in ITEM_SUBCATEGORY_DATA.items():
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

            # Get current subcategory for logging
            old_subcategory = category_data.get_editor_property('sub_category')

            # Set the subcategory enum value
            category_data.set_editor_property('sub_category', new_subcategory)

            # Update ItemInformation with new category struct
            item_info.set_editor_property('category', category_data)

            # Update the asset's ItemInformation
            asset.set_editor_property('item_information', item_info)

            # Save the asset
            unreal.EditorAssetLibrary.save_loaded_asset(asset)

            old_name = get_subcategory_name(old_subcategory)
            new_name = get_subcategory_name(new_subcategory)
            log(f"  OK: {item_name}")
            log(f"      SubCategory: {old_name} -> {new_name}")
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
    """List current subcategory values for all items."""

    log("=" * 60)
    log("CURRENT ITEM SUBCATEGORY VALUES")
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
                if category_data:
                    category_value = category_data.get_editor_property('category')
                    subcategory_value = category_data.get_editor_property('sub_category')

                    cat_name = str(category_value).split('.')[-1] if category_value else "N/A"
                    subcat_name = get_subcategory_name(subcategory_value)
                    log(f"  {asset_name}: Category={cat_name}, SubCategory={subcat_name}")
                else:
                    log(f"  {asset_name}: No category data")
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
    success, errors = apply_subcategories()

    # Show results after
    log("")
    log("=== AFTER MIGRATION ===")
    log("")
    list_current_values()

    # Write output to file
    with open(OUTPUT_FILE, 'w') as f:
        f.write('\n'.join(OUTPUT_LINES))
    unreal.log_warning(f"Output written to {OUTPUT_FILE}")
