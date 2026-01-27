"""
Apply WeaponStatusEffectInfo data to weapon data assets.
Version 3: Using export_text/import_text approach for proper nested struct serialization.
"""

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/apply_weapon_statuseffects_log.txt"
log_lines = []

def log(msg):
    print(msg)
    unreal.log(msg)
    log_lines.append(str(msg))

# Weapon status effect data - using the EXACT text format from T3D export
# WeaponStatusEffectInfo_118_50E270B04A50041AE79D29AC86F423A2=(("/Game/...", (Rank_4_..., BuildupAmount_8_...)))
WEAPON_STATUS_EFFECT_TEXT = {
    "/Game/SoulslikeFramework/Data/Items/DA_PoisonSword": {
        "status_effect_path": "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Poison",
        "rank": 3,
        "buildup_amount": 50.0
    }
}

def apply_weapon_status_effects():
    log("=" * 60)
    log("Applying Weapon Status Effect Data (v3)")
    log("=" * 60)

    success_count = 0
    fail_count = 0

    for weapon_path, effect_data in WEAPON_STATUS_EFFECT_TEXT.items():
        log(f"\nProcessing: {weapon_path}")

        # Load weapon data asset
        weapon = unreal.EditorAssetLibrary.load_asset(weapon_path)
        if not weapon:
            log(f"  ERROR: Could not load weapon asset")
            fail_count += 1
            continue

        log(f"  Loaded: {weapon.get_class().get_name()}")

        try:
            # Load the status effect asset
            effect_asset = unreal.EditorAssetLibrary.load_asset(effect_data["status_effect_path"])
            if not effect_asset:
                log(f"  ERROR: Could not load status effect")
                fail_count += 1
                continue

            log(f"  Status effect: {effect_asset.get_name()}")

            # Create the application struct
            application = unreal.SLFStatusEffectApplication()
            application.rank = effect_data["rank"]
            application.buildup_amount = effect_data["buildup_amount"]
            log(f"  Created application: Rank={application.rank}, Buildup={application.buildup_amount}")

            # Get the full struct path and create the map entry
            # The key is TObjectPtr<UPrimaryDataAsset>, value is FSLFStatusEffectApplication
            item_info = weapon.get_editor_property("item_information")
            equip_details = item_info.equipment_details  # Direct attribute access

            log(f"  equip_details type: {type(equip_details)}")
            log(f"  Current weapon_status_effect_info: {equip_details.weapon_status_effect_info}")

            # Create a new map with the effect
            new_map = {effect_asset: application}

            # Set on the equipment details - try direct assignment
            equip_details.weapon_status_effect_info = new_map
            log(f"  Set weapon_status_effect_info to map with 1 entry")

            # Set equipment_details back on item_info
            item_info.equipment_details = equip_details
            log(f"  Set equipment_details back")

            # Set item_information back on weapon
            weapon.set_editor_property("item_information", item_info)
            log(f"  Set item_information back")

            # Mark dirty and save
            weapon.modify()
            if unreal.EditorAssetLibrary.save_asset(weapon_path):
                log(f"  SAVED successfully!")

                # Verify immediately
                weapon2 = unreal.EditorAssetLibrary.load_asset(weapon_path)
                item_info2 = weapon2.get_editor_property("item_information")
                equip_details2 = item_info2.equipment_details
                status_map2 = equip_details2.weapon_status_effect_info
                log(f"  Verification - weapon_status_effect_info has {len(status_map2)} entries")
                if len(status_map2) > 0:
                    success_count += 1
                    for k, v in status_map2.items():
                        log(f"    Key: {k.get_name() if k else 'None'}, Rank={v.rank}, Buildup={v.buildup_amount}")
                else:
                    log(f"  WARNING: Data did not persist!")
                    fail_count += 1
            else:
                log(f"  ERROR: Save failed")
                fail_count += 1

        except Exception as e:
            log(f"  ERROR: {e}")
            import traceback
            log(f"  {traceback.format_exc()}")
            fail_count += 1

    log(f"\n" + "=" * 60)
    log(f"Results: {success_count} succeeded, {fail_count} failed")
    log("=" * 60)

# Run
apply_weapon_status_effects()

# Save log
with open(OUTPUT_FILE, 'w') as f:
    f.write('\n'.join(log_lines))
log(f"\nLog saved to: {OUTPUT_FILE}")
