"""
Apply WeaponStatusEffectInfo data to weapon data assets.
This restores status effect data lost during Blueprint migration.
"""

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/apply_weapon_statuseffects_log.txt"
log_lines = []

def log(msg):
    print(msg)
    unreal.log(msg)
    log_lines.append(str(msg))

# Weapon status effect data extracted from bp_only T3D exports
# Format: {weapon_path: {status_effect_path: (rank, buildup_amount)}}
WEAPON_STATUS_EFFECTS = {
    "/Game/SoulslikeFramework/Data/Items/DA_PoisonSword": {
        "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Poison": (3, 50.0)
    }
}

def apply_weapon_status_effects():
    log("=" * 60)
    log("Applying Weapon Status Effect Data")
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
            # Get ItemInformation
            item_info = weapon.get_editor_property("item_information")
            if not item_info:
                log(f"  ERROR: No item_information")
                fail_count += 1
                continue

            # Get EquipmentDetails
            equip_details = item_info.get_editor_property("equipment_details")
            if not equip_details:
                log(f"  ERROR: No equipment_details")
                fail_count += 1
                continue

            # Get or create WeaponStatusEffectInfo map
            status_effect_map = equip_details.get_editor_property("weapon_status_effect_info")
            if status_effect_map is None:
                # Create new map
                status_effect_map = {}

            log(f"  Current status effects: {len(status_effect_map) if status_effect_map else 0}")

            # Add each status effect
            for effect_path, (rank, buildup) in effects_data.items():
                # Load the status effect data asset
                effect_asset = unreal.EditorAssetLibrary.load_asset(effect_path)
                if not effect_asset:
                    log(f"  ERROR: Could not load status effect: {effect_path}")
                    continue

                log(f"  Adding: {effect_asset.get_name()} (Rank={rank}, Buildup={buildup})")

                # Create FSLFStatusEffectApplication struct
                application = unreal.SLFStatusEffectApplication()
                application.set_editor_property("rank", rank)
                application.set_editor_property("buildup_amount", buildup)

                # Add to map (map key is the UPrimaryDataAsset*)
                status_effect_map[effect_asset] = application

            # Set the map back on the equipment details
            equip_details.set_editor_property("weapon_status_effect_info", status_effect_map)

            # Set equipment details back on item info (may be needed due to copy semantics)
            item_info.set_editor_property("equipment_details", equip_details)

            # Set item info back on weapon
            weapon.set_editor_property("item_information", item_info)

            # Save the asset
            unreal.EditorAssetLibrary.save_asset(weapon_path)
            log(f"  SAVED successfully!")
            success_count += 1

        except Exception as e:
            log(f"  ERROR: {e}")
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
