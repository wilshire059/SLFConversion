import unreal

output = []
def log(msg):
    print(msg)
    output.append(str(msg))

log("=" * 70)
log("CHECKING WEAPON STATUS EFFECTS")
log("=" * 70)

# Check the poison sword (should have poison status effect)
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
            # Check ItemInformation.WeaponDetails.StatusEffects
            item_info = wp.get_editor_property("item_information")
            if item_info:
                weapon_details = item_info.get_editor_property("weapon_details")
                if weapon_details:
                    status_effects = weapon_details.get_editor_property("status_effects")
                    log(f"  StatusEffects type: {type(status_effects)}")
                    if status_effects:
                        log(f"  StatusEffects count: {len(status_effects) if hasattr(status_effects, '__len__') else 'N/A'}")
                        for key in status_effects:
                            log(f"    - {key}: {status_effects[key]}")
                    else:
                        log("  StatusEffects: Empty or None")
                else:
                    log("  WeaponDetails: None")
            else:
                log("  ItemInformation: None")
        except Exception as e:
            log(f"  Error: {e}")
    else:
        log("  Could not load asset")

# Write output
with open("C:/scripts/SLFConversion/migration_cache/weapon_status_check.txt", 'w') as f:
    f.write('\n'.join(output))

log("\nDone")
