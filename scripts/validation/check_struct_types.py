"""
Check what struct types are available in Python for status effects.
"""

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/check_struct_types_log.txt"
log_lines = []

def log(msg):
    print(msg)
    unreal.log(msg)
    log_lines.append(str(msg))

def check_structs():
    log("=" * 60)
    log("Checking Available Struct Types")
    log("=" * 60)

    # List of struct names to check
    struct_names = [
        "SLFStatusEffectApplication",
        "FSLFStatusEffectApplication",
        "StatusEffectApplication",
        "SLFEquipmentInfo",
        "FSLFEquipmentInfo",
        "EquipmentInfo",
        "SLFItemInfo",
        "FSLFItemInfo",
        "ItemInfo",
    ]

    log("\nChecking struct availability:")
    for name in struct_names:
        try:
            struct_type = getattr(unreal, name)
            log(f"  FOUND: {name}")
            # Try to instantiate
            try:
                instance = struct_type()
                log(f"    Can instantiate: Yes")
                # Get properties
                props = dir(instance)
                prop_list = [p for p in props if not p.startswith('_') and not callable(getattr(instance, p, None))]
                log(f"    Properties: {prop_list[:10]}...")  # First 10
            except Exception as e:
                log(f"    Can instantiate: No - {e}")
        except AttributeError:
            log(f"  NOT FOUND: {name}")

    log("\n\nSearching for 'StatusEffect' in unreal module:")
    for attr in dir(unreal):
        if 'statuseffect' in attr.lower():
            log(f"  {attr}")

    log("\n\nSearching for 'Application' in unreal module:")
    for attr in dir(unreal):
        if 'application' in attr.lower():
            log(f"  {attr}")

    # Check the actual weapon asset
    log("\n\nChecking weapon asset property types:")
    weapon = unreal.EditorAssetLibrary.load_asset("/Game/SoulslikeFramework/Data/Items/DA_PoisonSword")
    if weapon:
        item_info = weapon.get_editor_property("item_information")
        if item_info:
            log(f"  item_information type: {type(item_info)}")
            equip_details = item_info.get_editor_property("equipment_details")
            if equip_details:
                log(f"  equipment_details type: {type(equip_details)}")

                # Try getting the map type
                status_map = equip_details.get_editor_property("weapon_status_effect_info")
                log(f"  weapon_status_effect_info type: {type(status_map)}")

                # Check what fields equip_details has
                ed_props = [p for p in dir(equip_details) if not p.startswith('_')]
                log(f"  equipment_details properties: {ed_props}")

# Run
check_structs()

# Save log
with open(OUTPUT_FILE, 'w') as f:
    f.write('\n'.join(log_lines))
log(f"\nLog: {OUTPUT_FILE}")
