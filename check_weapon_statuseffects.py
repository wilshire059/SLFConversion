import unreal

output = []
def log(msg):
    print(msg)
    output.append(str(msg))

log("=" * 70)
log("CHECKING WEAPON STATUS EFFECTS")
log("=" * 70)

# Check all weapon data assets
weapon_paths = [
    "/Game/SoulslikeFramework/Data/Items/DA_Sword01",
    "/Game/SoulslikeFramework/Data/Items/DA_Greatsword",
    "/Game/SoulslikeFramework/Data/Items/DA_Katana",
    "/Game/SoulslikeFramework/Data/Items/DA_PoisonSword",
    "/Game/SoulslikeFramework/Data/Items/DA_VenomSword",
]

for path in weapon_paths:
    asset = unreal.EditorAssetLibrary.load_asset(path)
    if asset:
        log(f"\n{path.split('/')[-1]}:")

        # Try to get item_information.equipment_details.weapon_status_effect_info
        try:
            item_info = asset.get_editor_property("item_information")
            if item_info:
                equip_details = item_info.get_editor_property("equipment_details")
                if equip_details:
                    status_effects = equip_details.get_editor_property("weapon_status_effect_info")
                    if status_effects:
                        log(f"  WeaponStatusEffectInfo: {len(status_effects)} entries")
                        for key, value in status_effects.items():
                            log(f"    {key.get_name() if key else 'None'}: Rank={value.rank}, Buildup={value.buildup_amount}")
                    else:
                        log(f"  WeaponStatusEffectInfo: EMPTY or None")
                else:
                    log(f"  equipment_details: None")
            else:
                log(f"  item_information: None")
        except Exception as e:
            log(f"  Error: {e}")
    else:
        log(f"\n{path}: NOT FOUND")

# Write to file
with open("C:/scripts/SLFConversion/migration_cache/weapon_statuseffect_check.txt", 'w') as f:
    f.write('\n'.join(output))

log("\n" + "=" * 70)
log("Done!")
