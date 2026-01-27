# extract_stat_defaults_full.py
# Extract stat CDO defaults from bp_only project for ALL stat types
# Run against bp_only project

import unreal
import json
import os

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/stat_defaults_full.json"

# All stat paths to check (CORRECTED paths)
STAT_PATHS = {
    # Defense (Damage Negation) - B_DN_* naming
    "B_DN_Physical": "/Game/SoulslikeFramework/Data/Stats/Defense/B_DN_Physical",
    "B_DN_Fire": "/Game/SoulslikeFramework/Data/Stats/Defense/B_DN_Fire",
    "B_DN_Frost": "/Game/SoulslikeFramework/Data/Stats/Defense/B_DN_Frost",
    "B_DN_Holy": "/Game/SoulslikeFramework/Data/Stats/Defense/B_DN_Holy",
    "B_DN_Lightning": "/Game/SoulslikeFramework/Data/Stats/Defense/B_DN_Lightning",
    "B_DN_Magic": "/Game/SoulslikeFramework/Data/Stats/Defense/B_DN_Magic",

    # Resistances
    "B_Resistance_Focus": "/Game/SoulslikeFramework/Data/Stats/Defense/B_Resistance_Focus",
    "B_Resistance_Immunity": "/Game/SoulslikeFramework/Data/Stats/Defense/B_Resistance_Immunity",
    "B_Resistance_Robustness": "/Game/SoulslikeFramework/Data/Stats/Defense/B_Resistance_Robustness",
    "B_Resistance_Vitality": "/Game/SoulslikeFramework/Data/Stats/Defense/B_Resistance_Vitality",

    # Attack Power - B_AP_* naming, in Secondary/AttackPower
    "B_AP_Physical": "/Game/SoulslikeFramework/Data/Stats/Secondary/AttackPower/B_AP_Physical",
    "B_AP_Fire": "/Game/SoulslikeFramework/Data/Stats/Secondary/AttackPower/B_AP_Fire",
    "B_AP_Frost": "/Game/SoulslikeFramework/Data/Stats/Secondary/AttackPower/B_AP_Frost",
    "B_AP_Holy": "/Game/SoulslikeFramework/Data/Stats/Secondary/AttackPower/B_AP_Holy",
    "B_AP_Lightning": "/Game/SoulslikeFramework/Data/Stats/Secondary/AttackPower/B_AP_Lightning",
    "B_AP_Magic": "/Game/SoulslikeFramework/Data/Stats/Secondary/AttackPower/B_AP_Magic",

    # Backend stats
    "B_Poise": "/Game/SoulslikeFramework/Data/Stats/_Backend/B_Poise",
    "B_Stance": "/Game/SoulslikeFramework/Data/Stats/_Backend/B_Stance",
    "B_IncantationPower": "/Game/SoulslikeFramework/Data/Stats/_Backend/B_IncantationPower",

    # Misc
    "B_Weight": "/Game/SoulslikeFramework/Data/Stats/Misc/B_Weight",

    # Primary
    "B_Vigor": "/Game/SoulslikeFramework/Data/Stats/Primary/B_Vigor",
    "B_Strength": "/Game/SoulslikeFramework/Data/Stats/Primary/B_Strength",
    "B_Dexterity": "/Game/SoulslikeFramework/Data/Stats/Primary/B_Dexterity",
    "B_Intelligence": "/Game/SoulslikeFramework/Data/Stats/Primary/B_Intelligence",
    "B_Faith": "/Game/SoulslikeFramework/Data/Stats/Primary/B_Faith",
    "B_Arcane": "/Game/SoulslikeFramework/Data/Stats/Primary/B_Arcane",
    "B_Mind": "/Game/SoulslikeFramework/Data/Stats/Primary/B_Mind",
    "B_Endurance": "/Game/SoulslikeFramework/Data/Stats/Primary/B_Endurance",

    # Secondary
    "B_HP": "/Game/SoulslikeFramework/Data/Stats/Secondary/B_HP",
    "B_Stamina": "/Game/SoulslikeFramework/Data/Stats/Secondary/B_Stamina",
    "B_FP": "/Game/SoulslikeFramework/Data/Stats/Secondary/B_FP",
}

def extract_stat_info(bp_path):
    """Extract StatInfo from a stat Blueprint CDO"""
    try:
        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            print(f"Could not load: {bp_path}")
            return None

        gen_class = bp.generated_class()
        if not gen_class:
            print(f"No generated class: {bp_path}")
            return None

        cdo = unreal.get_default_object(gen_class)
        if not cdo:
            print(f"No CDO: {bp_path}")
            return None

        # Get StatInfo struct
        try:
            stat_info = cdo.get_editor_property("stat_info") or cdo.get_editor_property("StatInfo")
            if not stat_info:
                print(f"No stat_info property: {bp_path}")
                return None

            result = {
                "path": bp_path,
                "tag": str(stat_info.tag) if hasattr(stat_info, 'tag') else "",
                "display_name": str(stat_info.display_name) if hasattr(stat_info, 'display_name') else "",
                "current_value": float(stat_info.current_value) if hasattr(stat_info, 'current_value') else 0.0,
                "max_value": float(stat_info.max_value) if hasattr(stat_info, 'max_value') else 0.0,
                "show_max_value": bool(stat_info.show_max_value) if hasattr(stat_info, 'show_max_value') else False,
                "display_as_percent": bool(stat_info.display_as_percent) if hasattr(stat_info, 'display_as_percent') else False,
            }

            print(f"OK: {bp_path}")
            print(f"    Tag: {result['tag']}")
            print(f"    Display: {result['display_name']}")
            print(f"    Current: {result['current_value']}")
            print(f"    Max: {result['max_value']}")

            return result

        except Exception as e:
            print(f"Error getting stat_info: {bp_path}: {e}")
            return None

    except Exception as e:
        print(f"Error loading: {bp_path}: {e}")
        return None

def main():
    print("=" * 60)
    print("Extracting ALL stat defaults from bp_only")
    print("=" * 60)

    results = {}

    for name, path in STAT_PATHS.items():
        result = extract_stat_info(path)
        if result:
            results[name] = result

    # Save to JSON
    os.makedirs(os.path.dirname(OUTPUT_FILE), exist_ok=True)
    with open(OUTPUT_FILE, 'w') as f:
        json.dump(results, f, indent=2)

    print("=" * 60)
    print(f"Saved {len(results)} stat defaults to {OUTPUT_FILE}")
    print("=" * 60)

if __name__ == "__main__":
    main()
