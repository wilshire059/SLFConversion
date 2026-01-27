"""
Apply weapon stat data from cached JSON to migrated weapon data assets.
Uses USLFAutomationLibrary::ApplyWeaponStatData
"""
import unreal
import json

# Output file for results
OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/apply_weapon_stats_output.txt"
output_lines = []


def log(msg):
    """Log message to file"""
    output_lines.append(msg)


def log_warn(msg):
    """Log warning to file"""
    output_lines.append(f"WARNING: {msg}")


def log_error(msg):
    """Log error to file"""
    output_lines.append(f"ERROR: {msg}")


def write_output():
    """Write all output lines to file"""
    with open(OUTPUT_FILE, 'w', encoding='utf-8') as f:
        for line in output_lines:
            f.write(line + '\n')


# Load parsed weapon data
CACHE_FILE = "C:/scripts/SLFConversion/migration_cache/weapon_stats_parsed.json"

# Map weapon data asset names to their asset paths
WEAPON_ASSET_PATHS = {
    "DA_Katana": "/Game/SoulslikeFramework/Data/Items/DA_Katana",
    "DA_Greatsword": "/Game/SoulslikeFramework/Data/Items/DA_Greatsword",
    "DA_BossMace": "/Game/SoulslikeFramework/Data/Items/DA_BossMace",
    "DA_PoisonSword": "/Game/SoulslikeFramework/Data/Items/DA_PoisonSword",
    "DA_Sword01": "/Game/SoulslikeFramework/Data/Items/DA_Sword01",
    "DA_Sword02": "/Game/SoulslikeFramework/Data/Items/DA_Sword02",
    "DA_Shield": "/Game/SoulslikeFramework/Data/Items/DA_Shield",
}

# Scaling grade names mapping
SCALING_GRADES = {
    "S": "S",
    "A": "A",
    "B": "B",
    "C": "C",
    "D": "D",
    "E": "E"
}


def apply_weapon_stats():
    """Apply weapon stats from cached JSON to data assets"""

    # Load cached data
    try:
        with open(CACHE_FILE, 'r', encoding='utf-8') as f:
            weapon_data = json.load(f)
    except Exception as e:
        log_error(f"Could not load cache file: {e}")
        return

    log(f"Loaded {len(weapon_data)} weapons from cache")

    success_count = 0
    fail_count = 0

    for weapon_name, data in weapon_data.items():
        asset_path = WEAPON_ASSET_PATHS.get(weapon_name)
        if not asset_path:
            log_warn(f"SKIP: {weapon_name} - no asset path mapping")
            continue

        log(f"Processing {weapon_name}...")

        # Prepare parameters for C++ function
        has_stat_scaling = data.get('has_stat_scaling', False)
        has_stat_requirement = data.get('has_stat_requirement', False)

        # Format scaling stats as "Stat:Grade" strings
        scaling_stats = []
        if 'scaling_info' in data:
            for stat_name, grade in data['scaling_info'].items():
                scaling_stats.append(f"{stat_name}:{grade}")

        # Format requirement stats as "Stat:Value" strings
        requirement_stats = []
        if 'stat_requirement_info' in data:
            for stat_name, value in data['stat_requirement_info'].items():
                requirement_stats.append(f"{stat_name}:{value}")

        # Format stat changes as "DamageType:Value" strings
        stat_changes = []
        if 'stat_changes' in data:
            for damage_type, value in data['stat_changes'].items():
                stat_changes.append(f"{damage_type}:{value}")

        log(f"  HasScaling: {has_stat_scaling}, Scaling: {scaling_stats}")
        log(f"  HasRequirement: {has_stat_requirement}, Requirements: {requirement_stats}")
        log(f"  StatChanges: {stat_changes}")

        # Call C++ function
        try:
            result = unreal.SLFAutomationLibrary.apply_weapon_stat_data(
                asset_path,
                has_stat_scaling,
                scaling_stats,
                has_stat_requirement,
                requirement_stats,
                stat_changes
            )

            if result:
                log(f"  OK: Applied stats to {weapon_name}")
                success_count += 1
            else:
                log_error(f"  FAIL: Could not apply stats to {weapon_name}")
                fail_count += 1

        except Exception as e:
            log_error(f"  ERROR: {e}")
            fail_count += 1

    log(f"SUMMARY: {success_count} succeeded, {fail_count} failed")
    write_output()


if __name__ == "__main__":
    apply_weapon_stats()
