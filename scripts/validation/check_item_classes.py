# check_item_classes.py
# Diagnose actual UE classes of item data assets

import unreal

LOG_FILE = "C:/scripts/SLFConversion/migration_cache/item_classes_check.txt"
log_lines = []

def log(msg):
    print(msg)
    unreal.log(str(msg))
    log_lines.append(str(msg))

# Items to check - both working and non-working
ITEMS_TO_CHECK = [
    # Working items
    "/Game/SoulslikeFramework/Data/Items/DA_HealthFlask.DA_HealthFlask",
    "/Game/SoulslikeFramework/Data/Items/DA_MagicSpell.DA_MagicSpell",
    "/Game/SoulslikeFramework/Data/Items/DA_ThrowingKnife.DA_ThrowingKnife",
    # New spell items (may not be working)
    "/Game/SoulslikeFramework/Data/Items/Spells/DA_FireballSpell.DA_FireballSpell",
    "/Game/SoulslikeFramework/Data/Items/Spells/DA_IceShardSpell.DA_IceShardSpell",
    "/Game/SoulslikeFramework/Data/Items/Spells/DA_LightningBoltSpell.DA_LightningBoltSpell",
]

log("=" * 70)
log("ITEM CLASS DIAGNOSTIC")
log("=" * 70)

for path in ITEMS_TO_CHECK:
    log(f"\n{path}")
    log("-" * 70)

    asset = unreal.EditorAssetLibrary.load_asset(path)
    if not asset:
        log("  ERROR: Could not load asset")
        continue

    # Get class info
    asset_class = asset.get_class()
    log(f"  Asset Class: {asset_class.get_name()}")
    log(f"  Asset Class Path: {asset_class.get_path_name()}")

    # Get parent class
    parent_class = asset_class.get_super_class()
    if parent_class:
        log(f"  Parent Class: {parent_class.get_name()}")
        log(f"  Parent Class Path: {parent_class.get_path_name()}")

        # Get grandparent
        grandparent = parent_class.get_super_class()
        if grandparent:
            log(f"  Grandparent Class: {grandparent.get_name()}")

    # Check if it's a PDA_Item
    try:
        pda_item_class = unreal.load_class(None, "/Script/SLFConversion.PDA_Item")
        if pda_item_class:
            is_pda_item = asset_class.is_child_of(pda_item_class)
            log(f"  Is UPDA_Item or subclass: {is_pda_item}")
    except Exception as e:
        log(f"  Could not check UPDA_Item inheritance: {e}")

    # Check category
    try:
        item_info = asset.get_editor_property("item_information")
        if item_info:
            category_data = item_info.get_editor_property("category")
            if category_data:
                cat = category_data.get_editor_property("category")
                log(f"  Category enum: {cat}")
    except Exception as e:
        log(f"  Could not get category: {e}")

log("\n" + "=" * 70)
log("DIAGNOSTIC COMPLETE")
log("=" * 70)

# Save log
with open(LOG_FILE, 'w', encoding='utf-8') as f:
    f.write('\n'.join(log_lines))
log(f"\nLog saved to: {LOG_FILE}")
