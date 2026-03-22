import unreal

output = []
def log(msg):
    print(msg)
    output.append(str(msg))

log("=" * 70)
log("CHECKING WEAPON STATUS EFFECTS IN SLFCONVERSION")
log("=" * 70)

# Check all weapon data assets
weapon_paths = [
    "/Game/SoulslikeFramework/Data/Items/DA_Sword01",
    "/Game/SoulslikeFramework/Data/Items/DA_Greatsword",
    "/Game/SoulslikeFramework/Data/Items/DA_Katana",
    "/Game/SoulslikeFramework/Data/Items/DA_PoisonSword",
]

for path in weapon_paths:
    asset = unreal.EditorAssetLibrary.load_asset(path)
    if asset:
        log(f"\n{path.split('/')[-1]}:")
        log(f"  Class: {asset.get_class().get_name()}")

        # Try to get item_information.equipment_details.weapon_status_effect_info
        try:
            item_info = asset.get_editor_property("item_information")
            if item_info:
                log(f"  Has ItemInformation: Yes")
                equip_details = item_info.get_editor_property("equipment_details")
                if equip_details:
                    log(f"  Has EquipmentDetails: Yes")
                    status_effects = equip_details.get_editor_property("weapon_status_effect_info")
                    if status_effects:
                        log(f"  WeaponStatusEffectInfo: {len(status_effects)} entries")
                        for key, value in status_effects.items():
                            if key:
                                effect_name = key.get_name()
                                rank = value.get_editor_property("rank") if hasattr(value, 'get_editor_property') else 0
                                buildup = value.get_editor_property("buildup_amount") if hasattr(value, 'get_editor_property') else 0
                                log(f"    {effect_name}: Rank={rank}, Buildup={buildup}")
                    else:
                        log(f"  WeaponStatusEffectInfo: EMPTY or None")
                else:
                    log(f"  EquipmentDetails: None")
            else:
                log(f"  ItemInformation: None")
        except Exception as e:
            log(f"  Error: {e}")
    else:
        log(f"\n{path}: NOT FOUND")

# Also check status effect data assets
log("\n" + "=" * 70)
log("CHECKING STATUS EFFECT DATA ASSETS")
log("=" * 70)

se_paths = [
    "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Bleed",
    "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Poison",
]

for path in se_paths:
    asset = unreal.EditorAssetLibrary.load_asset(path)
    if asset:
        log(f"\n{path.split('/')[-1]}:")
        log(f"  Class: {asset.get_class().get_name()}")
        log(f"  Valid: Yes")
    else:
        log(f"\n{path}: NOT FOUND")

# Write to file
with open("C:/scripts/SLFConversion/migration_cache/weapon_statuseffect_check_current.txt", 'w') as f:
    f.write('\n'.join(output))

log("\n" + "=" * 70)
log("Done! Saved to migration_cache/weapon_statuseffect_check_current.txt")
