"""Compare weapon vs armor struct loading"""
import unreal

def log(msg):
    unreal.log_warning(str(msg))

WEAPON = "/Game/SoulslikeFramework/Data/Items/DA_Sword01"
ARMOR = "/Game/SoulslikeFramework/Data/Items/DA_ExampleArmor"

log("=" * 60)
log("COMPARING WEAPON vs ARMOR")
log("=" * 60)

for path in [WEAPON, ARMOR]:
    name = path.split('/')[-1]
    log(f"\n=== {name} ===")

    asset = unreal.EditorAssetLibrary.load_asset(path)
    if not asset:
        log("  FAILED TO LOAD")
        continue

    log(f"  Asset class: {asset.get_class().get_name()}")
    log(f"  Asset parent: {asset.get_class().get_super_class().get_name() if asset.get_class().get_super_class() else 'None'}")

    try:
        item_info = asset.get_editor_property('item_information')
        log(f"  ItemInfo type: {type(item_info)}")
        display_name = item_info.get_editor_property('display_name')
        log(f"  DisplayName: '{display_name}'")

        equip_details = item_info.get_editor_property('equipment_details')
        equip_slots = equip_details.get_editor_property('equip_slots')
        log(f"  EquipSlots: {equip_slots}")
    except Exception as e:
        log(f"  ERROR: {e}")

log("\n" + "=" * 60)
