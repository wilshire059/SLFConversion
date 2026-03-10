import unreal

output = []
def log(msg):
    print(msg)
    output.append(str(msg))

log("=" * 70)
log("CHECKING DA_GREATSWORD STATUS EFFECTS (POST-FIX)")
log("=" * 70)

# Check DA_Greatsword
wp = unreal.EditorAssetLibrary.load_asset("/Game/SoulslikeFramework/Data/Items/DA_Greatsword")
if wp:
    try:
        item_info = wp.get_editor_property("item_information")
        if item_info:
            equip_details = item_info.get_editor_property("equipment_details")
            if equip_details:
                status_effects = equip_details.get_editor_property("weapon_status_effect_info")
                if status_effects and len(status_effects) > 0:
                    log(f"DA_Greatsword StatusEffects: {len(status_effects)} entries")
                    for key, value in status_effects.items():
                        log(f"  - {key.get_name() if key else 'None'}: Rank={value.get_editor_property('rank')}, Buildup={value.get_editor_property('buildup_amount')}")
                    log("\n*** STATUS EFFECTS RESTORED SUCCESSFULLY! ***")
                else:
                    log("DA_Greatsword: NO STATUS EFFECTS!")
    except Exception as e:
        log(f"Error: {e}")
else:
    log("Could not load DA_Greatsword")

# Write output
with open("C:/scripts/SLFConversion/migration_cache/greatsword_status.txt", 'w') as f:
    f.write('\n'.join(output))

log("\nDone")
