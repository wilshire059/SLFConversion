"""
Apply WeaponStatusEffectInfo data to weapon data assets.
This restores status effect data lost during Blueprint migration.
Reads from cached JSON extracted from bp_only backup.
"""

import unreal
import json

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/apply_weapon_statuseffects_log.txt"
INPUT_FILE = "C:/scripts/SLFConversion/migration_cache/weapon_statuseffect_data.json"
log_lines = []

def log(msg):
    print(msg)
    unreal.log(msg)
    log_lines.append(str(msg))

def load_weapon_status_effects():
    """Load weapon status effect data from JSON cache"""
    try:
        with open(INPUT_FILE, 'r') as f:
            data = json.load(f)
        log(f"Loaded {len(data)} weapons from cache")
        return data
    except Exception as e:
        log(f"ERROR loading cache: {e}")
        return {}

def apply_weapon_status_effects():
    log("=" * 60)
    log("Applying Weapon Status Effect Data using C++ Automation")
    log("=" * 60)

    # Load data from JSON
    weapon_data = load_weapon_status_effects()
    if not weapon_data:
        log("No weapon data to apply!")
        return

    success_count = 0
    fail_count = 0

    for weapon_path, effects_info in weapon_data.items():
        log(f"\nProcessing: {weapon_path}")

        # Prepare arrays for C++ function
        effects_list = effects_info.get("effects", [])
        if not effects_list:
            log(f"  No effects to apply")
            continue

        status_effect_paths = []
        ranks = []
        buildup_amounts = []

        for effect_data in effects_list:
            effect_path = effect_data.get("effect_path", "")
            effect_name = effect_data.get("effect_name", "")
            rank = effect_data.get("rank", 1)
            buildup = effect_data.get("buildup_amount", 50.0)

            # Extract clean path (remove .AssetName suffix if present)
            if "." in effect_path:
                clean_path = effect_path.rsplit(".", 1)[0]
            else:
                clean_path = effect_path

            status_effect_paths.append(clean_path)
            ranks.append(rank)
            buildup_amounts.append(buildup)

            log(f"  Effect: {effect_name} (Rank={rank}, Buildup={buildup})")

        try:
            # Use C++ automation function for reliable TMap handling
            result = unreal.SLFAutomationLibrary.apply_weapon_status_effects(
                weapon_path,
                status_effect_paths,
                ranks,
                buildup_amounts
            )

            if result:
                log(f"  SUCCESS!")
                success_count += 1
            else:
                log(f"  FAILED (C++ function returned false)")
                fail_count += 1

        except Exception as e:
            log(f"  ERROR: {e}")
            import traceback
            log(f"  Traceback: {traceback.format_exc()}")
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
