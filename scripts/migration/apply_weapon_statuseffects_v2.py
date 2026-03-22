"""
Apply WeaponStatusEffectInfo data to weapon data assets.
Version 2: Attempt different struct creation approach.
"""

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/apply_weapon_statuseffects_log.txt"
log_lines = []

def log(msg):
    print(msg)
    unreal.log(msg)
    log_lines.append(str(msg))

# Weapon status effect data extracted from bp_only T3D exports
WEAPON_STATUS_EFFECTS = {
    "/Game/SoulslikeFramework/Data/Items/DA_PoisonSword": {
        "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Poison": (3, 50.0)
    }
}

def apply_weapon_status_effects():
    log("=" * 60)
    log("Applying Weapon Status Effect Data (v2)")
    log("=" * 60)

    success_count = 0
    fail_count = 0

    for weapon_path, effects_data in WEAPON_STATUS_EFFECTS.items():
        log(f"\nProcessing: {weapon_path}")

        # Load weapon data asset
        weapon = unreal.EditorAssetLibrary.load_asset(weapon_path)
        if not weapon:
            log(f"  ERROR: Could not load weapon asset")
            fail_count += 1
            continue

        log(f"  Loaded: {weapon.get_class().get_name()}")

        try:
            # Get ItemInformation (this is a copy)
            item_info = weapon.get_editor_property("item_information")
            if not item_info:
                log(f"  ERROR: No item_information")
                fail_count += 1
                continue

            # Get EquipmentDetails (this is also a copy)
            equip_details = item_info.get_editor_property("equipment_details")
            if not equip_details:
                log(f"  ERROR: No equipment_details")
                fail_count += 1
                continue

            log(f"  EquipmentDetails type: {type(equip_details)}")

            # Create the status effect map
            status_effect_map = {}

            for effect_path, (rank, buildup) in effects_data.items():
                effect_asset = unreal.EditorAssetLibrary.load_asset(effect_path)
                if not effect_asset:
                    log(f"  ERROR: Could not load status effect: {effect_path}")
                    continue

                log(f"  Adding: {effect_asset.get_name()} (Rank={rank}, Buildup={buildup})")

                # Try creating the struct directly
                try:
                    # Check what struct types are available
                    log(f"  Looking for struct type...")

                    # Try different possible struct names
                    struct_names = [
                        "SLFStatusEffectApplication",
                        "FSLFStatusEffectApplication",
                        "StatusEffectApplication",
                    ]

                    application = None
                    for name in struct_names:
                        try:
                            application = getattr(unreal, name)()
                            log(f"    Found struct: {name}")
                            break
                        except AttributeError:
                            pass

                    if application is None:
                        log(f"  ERROR: Could not find status effect application struct")
                        continue

                    application.set_editor_property("rank", rank)
                    application.set_editor_property("buildup_amount", buildup)

                    status_effect_map[effect_asset] = application
                    log(f"  Created entry in map")

                except Exception as e:
                    log(f"  ERROR creating struct: {e}")
                    continue

            if len(status_effect_map) == 0:
                log(f"  No status effects to apply")
                fail_count += 1
                continue

            # Set the map on equipment details
            log(f"  Setting weapon_status_effect_info with {len(status_effect_map)} entries")
            equip_details.set_editor_property("weapon_status_effect_info", status_effect_map)

            # Critical: Copy the modified struct back through the chain
            log(f"  Setting equipment_details back on item_information")
            item_info.set_editor_property("equipment_details", equip_details)

            log(f"  Setting item_information back on weapon")
            weapon.set_editor_property("item_information", item_info)

            # Force mark dirty and save
            weapon.modify()
            if unreal.EditorAssetLibrary.save_asset(weapon_path):
                log(f"  SAVED successfully!")
                success_count += 1
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
