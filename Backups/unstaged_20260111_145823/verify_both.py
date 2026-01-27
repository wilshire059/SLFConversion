"""Verify both Niagara and EquipSlots are set."""
import unreal

def log(msg):
    unreal.log_warning(str(msg))

ARMOR_PATHS = [
    "/Game/SoulslikeFramework/Data/Items/DA_ExampleArmor",
    "/Game/SoulslikeFramework/Data/Items/DA_ExampleHelmet",
]

log("=" * 60)
log("VERIFYING NIAGARA + EQUIP SLOTS")
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

    # Check EquipSlots
    try:
        item_info = asset.get_editor_property('item_information')
        equip_details = item_info.get_editor_property('equipment_details')
        equip_slots = equip_details.get_editor_property('equip_slots')
        log(f"  EquipSlots: {type(equip_slots).__name__}")
    except Exception as e:
        log(f"  EquipSlots: ERROR - {e}")

log("\n" + "=" * 60)
