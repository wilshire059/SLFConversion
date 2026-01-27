"""Verify equip slots are set on armor items."""
import unreal

def log(msg):
    unreal.log_warning(str(msg))

ARMOR_PATHS = [
    "/Game/SoulslikeFramework/Data/Items/DA_ExampleArmor",
    "/Game/SoulslikeFramework/Data/Items/DA_ExampleHelmet",
    "/Game/SoulslikeFramework/Data/Items/DA_ExampleBracers",
]

log("=" * 60)
log("VERIFYING EQUIP SLOTS")
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
        equip_details = item_info.get_editor_property('equipment_details')
        equip_slots = equip_details.get_editor_property('equip_slots')

        # FGameplayTagContainer has to_string_simple()
        if hasattr(equip_slots, 'to_string_simple'):
            log(f"  EquipSlots: {equip_slots.to_string_simple()}")
        elif hasattr(equip_slots, 'num'):
            log(f"  EquipSlots count: {equip_slots.num()}")
        else:
            log(f"  EquipSlots type: {type(equip_slots)}")

    except Exception as e:
        log(f"  [ERROR] {e}")

log("\n" + "=" * 60)
