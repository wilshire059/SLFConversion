"""Verify ALL item data is preserved."""
import unreal

def log(msg):
    unreal.log_warning(str(msg))

ARMOR_PATHS = [
    "/Game/SoulslikeFramework/Data/Items/DA_ExampleArmor",
    "/Game/SoulslikeFramework/Data/Items/DA_ExampleHelmet",
]

log("=" * 60)
log("VERIFYING ALL ITEM DATA")
log("=" * 60)

for path in ARMOR_PATHS:
    name = path.split('/')[-1]
    log(f"\n--- {name} ---")

    asset = unreal.EditorAssetLibrary.load_asset(path)
    if not asset:
        log("  [ERROR] Failed to load")
        continue

    # Check Niagara on C++ property
    try:
        niagara = asset.get_editor_property('world_niagara_system')
        if niagara:
            log(f"  Niagara: OK ({niagara.get_name()})")
        else:
            log("  Niagara: MISSING")
    except Exception as e:
        log(f"  Niagara: ERROR - {e}")

    # Check Icon
    try:
        icon = asset.get_editor_property('icon_small')
        if icon:
            log(f"  Icon: OK ({icon.get_name()})")
        else:
            log("  Icon: MISSING")
    except Exception as e:
        log(f"  Icon: ERROR - {e}")

    # Check Display Name
    try:
        item_info = asset.get_editor_property('item_information')
        display_name = item_info.get_editor_property('display_name')
        if display_name:
            log(f"  DisplayName: OK ({display_name})")
        else:
            log("  DisplayName: MISSING")
    except Exception as e:
        log(f"  DisplayName: ERROR - {e}")

    # Check EquipSlots
    try:
        equip_details = item_info.get_editor_property('equipment_details')
        equip_slots = equip_details.get_editor_property('equip_slots')
        log(f"  EquipSlots: OK ({type(equip_slots).__name__})")
    except Exception as e:
        log(f"  EquipSlots: ERROR - {e}")

    # Check Category
    try:
        category = asset.get_editor_property('item_category')
        log(f"  Category: {category}")
    except Exception as e:
        log(f"  Category: ERROR - {e}")

log("\n" + "=" * 60)
