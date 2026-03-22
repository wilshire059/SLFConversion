"""
Extract ALL stat CDO defaults from bp_only project.
Includes: Primary, Secondary (HP/Stamina/FP), AttackPower, Defense, Resistances.

Run on bp_only project:
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/bp_only/bp_only.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/extract_all_stat_defaults.py" ^
  -stdout -unattended -nosplash
"""
import unreal
import json
import os

OUTPUT_PATH = "C:/scripts/SLFConversion/migration_cache/all_stat_defaults.json"

# All stat paths to extract
STAT_PATHS = [
    # Primary Stats
    "/Game/SoulslikeFramework/Data/Stats/Primary/B_Vigor",
    "/Game/SoulslikeFramework/Data/Stats/Primary/B_Strength",
    "/Game/SoulslikeFramework/Data/Stats/Primary/B_Dexterity",
    "/Game/SoulslikeFramework/Data/Stats/Primary/B_Intelligence",
    "/Game/SoulslikeFramework/Data/Stats/Primary/B_Faith",
    "/Game/SoulslikeFramework/Data/Stats/Primary/B_Mind",
    "/Game/SoulslikeFramework/Data/Stats/Primary/B_Endurance",
    "/Game/SoulslikeFramework/Data/Stats/Primary/B_Arcane",

    # Secondary Stats
    "/Game/SoulslikeFramework/Data/Stats/Secondary/B_HP",
    "/Game/SoulslikeFramework/Data/Stats/Secondary/B_Stamina",
    "/Game/SoulslikeFramework/Data/Stats/Secondary/B_FP",

    # Attack Power Stats
    "/Game/SoulslikeFramework/Data/Stats/Secondary/AttackPower/B_AP_Physical",
    "/Game/SoulslikeFramework/Data/Stats/Secondary/AttackPower/B_AP_Magic",
    "/Game/SoulslikeFramework/Data/Stats/Secondary/AttackPower/B_AP_Fire",
    "/Game/SoulslikeFramework/Data/Stats/Secondary/AttackPower/B_AP_Frost",
    "/Game/SoulslikeFramework/Data/Stats/Secondary/AttackPower/B_AP_Lightning",
    "/Game/SoulslikeFramework/Data/Stats/Secondary/AttackPower/B_AP_Holy",

    # Defense (Damage Negation) Stats
    "/Game/SoulslikeFramework/Data/Stats/Defense/B_DN_Physical",
    "/Game/SoulslikeFramework/Data/Stats/Defense/B_DN_Magic",
    "/Game/SoulslikeFramework/Data/Stats/Defense/B_DN_Fire",
    "/Game/SoulslikeFramework/Data/Stats/Defense/B_DN_Frost",
    "/Game/SoulslikeFramework/Data/Stats/Defense/B_DN_Lightning",
    "/Game/SoulslikeFramework/Data/Stats/Defense/B_DN_Holy",

    # Resistance Stats
    "/Game/SoulslikeFramework/Data/Stats/Defense/B_Resistance_Focus",
    "/Game/SoulslikeFramework/Data/Stats/Defense/B_Resistance_Immunity",
    "/Game/SoulslikeFramework/Data/Stats/Defense/B_Resistance_Robustness",
    "/Game/SoulslikeFramework/Data/Stats/Defense/B_Resistance_Vitality",
]

def extract_stat_defaults():
    print("=" * 80)
    print("EXTRACTING ALL STAT CDO DEFAULTS FROM BP_ONLY")
    print("=" * 80)

    results = {}

    for stat_path in STAT_PATHS:
        stat_name = stat_path.split('/')[-1]
        print(f"\n--- {stat_name} ---")

        bp = unreal.EditorAssetLibrary.load_asset(stat_path)
        if not bp:
            print(f"  ERROR: Could not load {stat_path}")
            continue

        gen_class = bp.generated_class()
        if not gen_class:
            print(f"  ERROR: No generated class")
            continue

        cdo = unreal.get_default_object(gen_class)
        if not cdo:
            print(f"  ERROR: No CDO")
            continue

        result = {
            "path": stat_path,
            "class": gen_class.get_name(),
        }

        # Try to get StatInfo
        try:
            stat_info = cdo.get_editor_property('stat_info')
            if stat_info:
                result["tag"] = str(stat_info.get_editor_property('tag'))
                result["display_name"] = str(stat_info.get_editor_property('display_name'))
                result["current_value"] = float(stat_info.get_editor_property('current_value'))
                result["max_value"] = float(stat_info.get_editor_property('max_value'))
                result["show_max_value"] = bool(stat_info.get_editor_property('show_max_value'))

                print(f"  Tag: {result['tag']}")
                print(f"  Display: {result['display_name']}")
                print(f"  Current: {result['current_value']}")
                print(f"  Max: {result['max_value']}")
        except Exception as e:
            print(f"  Could not get stat_info: {e}")

        # Try to get StatBehavior for derivation info
        try:
            stat_behavior = cdo.get_editor_property('stat_behavior')
            if stat_behavior:
                stats_to_affect = stat_behavior.get_editor_property('stats_to_affect')
                if stats_to_affect and len(stats_to_affect) > 0:
                    result["stats_to_affect"] = []
                    for tag, affected_data in stats_to_affect.items():
                        entry = {"tag": str(tag)}
                        softcap_data = affected_data.get_editor_property('softcap_data')
                        if softcap_data:
                            entry["softcap_entries"] = []
                            for softcap in softcap_data:
                                sc = {
                                    "from_level": int(softcap.get_editor_property('from_level')),
                                    "until_level": int(softcap.get_editor_property('until_level')),
                                    "affect_max": bool(softcap.get_editor_property('affect_max_value')),
                                    "modifier": float(softcap.get_editor_property('modifier')),
                                }
                                entry["softcap_entries"].append(sc)
                        result["stats_to_affect"].append(entry)
                    print(f"  StatsToAffect: {len(result['stats_to_affect'])} entries")
        except Exception as e:
            pass  # Not all stats have StatBehavior/StatsToAffect

        results[stat_name] = result

    # Save results
    os.makedirs(os.path.dirname(OUTPUT_PATH), exist_ok=True)
    with open(OUTPUT_PATH, 'w') as f:
        json.dump(results, f, indent=2, default=str)

    print(f"\n\nSaved {len(results)} stats to: {OUTPUT_PATH}")
    print("=" * 80)

if __name__ == "__main__":
    extract_stat_defaults()
