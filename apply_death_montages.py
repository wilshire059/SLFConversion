# apply_death_montages.py
# Applies cached Death montage data to PDA_CombatReactionAnimData assets
#
# Run on SLFConversion project AFTER running extract_death_montages.py on bp_only:
# UnrealEditor-Cmd.exe "C:/scripts/SLFConversion/SLFConversion.uproject" -run=pythonscript -script="C:/scripts/SLFConversion/apply_death_montages.py" -stdout -unattended

import unreal
import json
import os

INPUT_FILE = "C:/scripts/SLFConversion/migration_cache/death_montages.json"

def apply_death_montages():
    """Apply cached death montage data to reaction animset data assets."""

    unreal.log_warning("=" * 70)
    unreal.log_warning("APPLYING DEATH MONTAGES TO REACTION ANIMSETS")
    unreal.log_warning("=" * 70)

    # Load cached data
    if not os.path.exists(INPUT_FILE):
        unreal.log_warning(f"[ERROR] Cache file not found: {INPUT_FILE}")
        unreal.log_warning("Run extract_death_montages.py on bp_only project first!")
        return

    with open(INPUT_FILE, 'r') as f:
        montage_data = json.load(f)

    unreal.log_warning(f"Loaded {len(montage_data)} animsets from cache")

    success_count = 0
    fail_count = 0

    for asset_name, data in montage_data.items():
        asset_path = data.get("path", "")
        death_montages = data.get("death_montages", {})

        unreal.log_warning(f"\n[{asset_name}]")
        unreal.log_warning(f"  Path: {asset_path}")
        unreal.log_warning(f"  Death montages: {len(death_montages)}")

        if not death_montages:
            unreal.log_warning(f"  [SKIP] No death montages to apply")
            continue

        # Convert to arrays for C++ function
        direction_names = []
        montage_paths = []

        for dir_name, montage_path in death_montages.items():
            direction_names.append(dir_name)
            montage_paths.append(montage_path)
            unreal.log_warning(f"    {dir_name} -> {montage_path}")

        # Call C++ automation function
        try:
            result = unreal.SLFAutomationLibrary.apply_death_montages_to_animset(
                asset_path, direction_names, montage_paths
            )

            if result:
                unreal.log_warning(f"  [OK] Applied {len(death_montages)} death montages")
                success_count += 1
            else:
                unreal.log_warning(f"  [FAILED] Could not apply death montages")
                fail_count += 1

        except Exception as e:
            unreal.log_warning(f"  [ERROR] {e}")
            fail_count += 1

    unreal.log_warning("")
    unreal.log_warning("=" * 70)
    unreal.log_warning(f"COMPLETE: {success_count} succeeded, {fail_count} failed")
    unreal.log_warning("=" * 70)


if __name__ == "__main__":
    apply_death_montages()
