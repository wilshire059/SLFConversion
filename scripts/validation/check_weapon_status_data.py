import unreal

output = []
def log(msg):
    print(msg)
    output.append(str(msg))

log("=" * 70)
log("CHECKING WEAPON STATUS EFFECT DATA (C++ Structure)")
log("=" * 70)

# Check the poison sword
weapons = [
    "/Game/SoulslikeFramework/Data/Items/DA_PoisonSword",
    "/Game/SoulslikeFramework/Data/Items/DA_Sword01",
    "/Game/SoulslikeFramework/Data/Items/DA_Katana",
]

for wp_path in weapons:
    log(f"\n{wp_path.split('/')[-1]}:")
    wp = unreal.EditorAssetLibrary.load_asset(wp_path)
    if wp:
        try:
            # Check if this is UPDA_Item
            log(f"  Class: {wp.get_class().get_name()}")

            # Get ItemInformation
            item_info = wp.get_editor_property("item_information")
            if item_info:
                log(f"  ItemInformation: Found")

                # Get EquipmentDetails from ItemInformation
                equip_details = item_info.get_editor_property("equipment_details")
                if equip_details:
                    log(f"  EquipmentDetails: Found")

                    # Get WeaponStatusEffectInfo
                    status_effects = equip_details.get_editor_property("weapon_status_effect_info")
                    if status_effects:
                        log(f"  WeaponStatusEffectInfo: {len(status_effects)} entries")
                        for key, value in status_effects.items():
                            log(f"    - Key: {key.get_name() if key else 'None'}")
                            if value:
                                log(f"      Rank: {value.get_editor_property('rank')}")
                                log(f"      BuildupAmount: {value.get_editor_property('buildup_amount')}")
                    else:
                        log("  WeaponStatusEffectInfo: EMPTY or None")
                else:
                    log("  EquipmentDetails: None")
            else:
                log("  ItemInformation: None")
        except Exception as e:
            log(f"  Error: {e}")
    else:
        log("  Could not load asset")

# Write output
with open("C:/scripts/SLFConversion/migration_cache/weapon_status_data_check.txt", 'w') as f:
    f.write('\n'.join(output))

log("\nDone")
