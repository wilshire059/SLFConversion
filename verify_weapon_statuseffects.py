"""
Verify WeaponStatusEffectInfo was applied to weapon data assets.
"""

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/verify_weapon_statuseffects_log.txt"
log_lines = []

def log(msg):
    print(msg)
    unreal.log(msg)
    log_lines.append(str(msg))

def verify():
    log("=" * 60)
    log("Verifying Weapon Status Effect Data")
    log("=" * 60)

    weapon_paths = [
        "/Game/SoulslikeFramework/Data/Items/DA_PoisonSword",
        "/Game/SoulslikeFramework/Data/Items/DA_Sword01",
    ]

    for weapon_path in weapon_paths:
        log(f"\nChecking: {weapon_path}")

        weapon = unreal.EditorAssetLibrary.load_asset(weapon_path)
        if not weapon:
            log(f"  NOT FOUND")
            continue

        log(f"  Class: {weapon.get_class().get_name()}")

        try:
            item_info = weapon.get_editor_property("item_information")
            if not item_info:
                log(f"  ItemInformation: None")
                continue

            equip_details = item_info.get_editor_property("equipment_details")
            if not equip_details:
                log(f"  EquipmentDetails: None")
                continue

            status_map = equip_details.get_editor_property("weapon_status_effect_info")
            if not status_map or len(status_map) == 0:
                log(f"  WeaponStatusEffectInfo: EMPTY")
                continue

            log(f"  WeaponStatusEffectInfo: {len(status_map)} entries")
            for key, value in status_map.items():
                key_name = key.get_name() if key else "None"
                rank = value.get_editor_property("rank")
                buildup = value.get_editor_property("buildup_amount")
                log(f"    {key_name}: Rank={rank}, BuildupAmount={buildup}")

        except Exception as e:
            log(f"  ERROR: {e}")

# Run
verify()

# Save log
with open(OUTPUT_FILE, 'w') as f:
    f.write('\n'.join(log_lines))
log(f"\nLog: {OUTPUT_FILE}")
