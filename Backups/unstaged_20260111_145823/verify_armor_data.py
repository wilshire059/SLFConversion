"""Verify armor assets have their data after restore"""
import unreal

def log(msg):
    unreal.log_warning(str(msg))

ARMOR_PATHS = [
    "/Game/SoulslikeFramework/Data/Items/DA_ExampleArmor",
    "/Game/SoulslikeFramework/Data/Items/DA_ExampleHelmet",
]

log("=" * 60)
log("VERIFYING ARMOR DATA AFTER RESTORE")
log("=" * 60)

for path in ARMOR_PATHS:
    name = path.split('/')[-1]
    log(f"\n--- {name} ---")

    asset = unreal.EditorAssetLibrary.load_asset(path)
    if not asset:
        log("  [ERROR] Failed to load")
        continue

    try:
        item_info = asset.get_editor_property('item_information')

        # Check DisplayName
        display_name = item_info.get_editor_property('display_name')
        log(f"  DisplayName: {display_name}")

        # Check EquipmentDetails
        equip_details = item_info.get_editor_property('equipment_details')

        # Check EquipSlots
        equip_slots = equip_details.get_editor_property('equip_slots')
        log(f"  EquipSlots: {equip_slots}")

        # Check if it has any tags
        if hasattr(equip_slots, 'num'):
            log(f"  EquipSlots count: {equip_slots.num()}")

    except Exception as e:
        log(f"  [ERROR] {e}")

log("\n" + "=" * 60)
