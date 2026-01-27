"""
Extract stat default values from Blueprint CDOs in the backup.
Run this against bp_only to get original values.
"""

import unreal
import json

def extract_stat_defaults():
    """Extract StatInfo defaults from all stat Blueprints."""

    results = {}

    # List of stat Blueprint paths to check
    stat_paths = [
        # Secondary stats
        "/Game/SoulslikeFramework/Data/Stats/Secondary/B_HP",
        "/Game/SoulslikeFramework/Data/Stats/Secondary/B_FP",
        "/Game/SoulslikeFramework/Data/Stats/Secondary/B_Stamina",
        "/Game/SoulslikeFramework/Data/Stats/Secondary/B_Poise",
        "/Game/SoulslikeFramework/Data/Stats/Secondary/B_Stance",
        "/Game/SoulslikeFramework/Data/Stats/Secondary/B_Weight",
        "/Game/SoulslikeFramework/Data/Stats/Secondary/B_Discovery",
        "/Game/SoulslikeFramework/Data/Stats/Secondary/B_DeathCurrency",

        # Primary stats
        "/Game/SoulslikeFramework/Data/Stats/Primary/B_Vigor",
        "/Game/SoulslikeFramework/Data/Stats/Primary/B_Mind",
        "/Game/SoulslikeFramework/Data/Stats/Primary/B_Endurance",
        "/Game/SoulslikeFramework/Data/Stats/Primary/B_Strength",
        "/Game/SoulslikeFramework/Data/Stats/Primary/B_Dexterity",
        "/Game/SoulslikeFramework/Data/Stats/Primary/B_Intelligence",
        "/Game/SoulslikeFramework/Data/Stats/Primary/B_Faith",
        "/Game/SoulslikeFramework/Data/Stats/Primary/B_Arcane",

        # Attack Power stats
        "/Game/SoulslikeFramework/Data/Stats/Secondary/AttackPower/B_AP_Physical",
        "/Game/SoulslikeFramework/Data/Stats/Secondary/AttackPower/B_AP_Magic",
        "/Game/SoulslikeFramework/Data/Stats/Secondary/AttackPower/B_AP_Fire",
        "/Game/SoulslikeFramework/Data/Stats/Secondary/AttackPower/B_AP_Frost",
        "/Game/SoulslikeFramework/Data/Stats/Secondary/AttackPower/B_AP_Lightning",
        "/Game/SoulslikeFramework/Data/Stats/Secondary/AttackPower/B_AP_Holy",

        # Defense/Damage Negation stats
        "/Game/SoulslikeFramework/Data/Stats/Defense/B_DN_Physical",
        "/Game/SoulslikeFramework/Data/Stats/Defense/B_DN_Magic",
        "/Game/SoulslikeFramework/Data/Stats/Defense/B_DN_Fire",
        "/Game/SoulslikeFramework/Data/Stats/Defense/B_DN_Frost",
        "/Game/SoulslikeFramework/Data/Stats/Defense/B_DN_Lightning",
        "/Game/SoulslikeFramework/Data/Stats/Defense/B_DN_Holy",

        # Resistance stats
        "/Game/SoulslikeFramework/Data/Stats/Resistance/B_Resistance_Immunity",
        "/Game/SoulslikeFramework/Data/Stats/Resistance/B_Resistance_Robustness",
        "/Game/SoulslikeFramework/Data/Stats/Resistance/B_Resistance_Focus",
        "/Game/SoulslikeFramework/Data/Stats/Resistance/B_Resistance_Vitality",

        # Backend stats
        "/Game/SoulslikeFramework/Data/Stats/Backend/B_IncantationPower",
        "/Game/SoulslikeFramework/Data/Stats/Backend/B_StatusEffectBuildup",
    ]

    for bp_path in stat_paths:
        bp_asset = unreal.load_asset(bp_path)
        if not bp_asset:
            print(f"SKIP: {bp_path} - not found")
            continue

        # Get the generated class and its CDO
        gen_class = bp_asset.generated_class()
        if not gen_class:
            print(f"SKIP: {bp_path} - no generated class")
            continue

        cdo = unreal.get_default_object(gen_class)
        if not cdo:
            print(f"SKIP: {bp_path} - no CDO")
            continue

        # Extract stat name from path
        stat_name = bp_path.split("/")[-1]

        stat_data = {
            "path": bp_path,
            "class_name": gen_class.get_name(),
        }

        # Try to get StatInfo properties
        try:
            stat_info = cdo.get_editor_property('stat_info')
            if stat_info:
                stat_data["stat_info"] = {
                    "tag": str(stat_info.get_editor_property('tag')) if hasattr(stat_info, 'get_editor_property') else "",
                    "display_name": str(stat_info.get_editor_property('display_name')) if hasattr(stat_info, 'get_editor_property') else "",
                    "current_value": float(stat_info.get_editor_property('current_value')) if hasattr(stat_info, 'get_editor_property') else 0.0,
                    "max_value": float(stat_info.get_editor_property('max_value')) if hasattr(stat_info, 'get_editor_property') else 0.0,
                    "show_max_value": bool(stat_info.get_editor_property('show_max_value')) if hasattr(stat_info, 'get_editor_property') else False,
                    "display_as_percent": bool(stat_info.get_editor_property('display_as_percent')) if hasattr(stat_info, 'get_editor_property') else False,
                }
        except Exception as e:
            stat_data["stat_info_error"] = str(e)

        # Try to get min_value
        try:
            stat_data["min_value"] = float(cdo.get_editor_property('min_value'))
        except:
            pass

        # Try to get only_max_value_relevant
        try:
            stat_data["only_max_value_relevant"] = bool(cdo.get_editor_property('only_max_value_relevant'))
        except:
            pass

        # Try to get show_max_value_on_level_up
        try:
            stat_data["show_max_value_on_level_up"] = bool(cdo.get_editor_property('show_max_value_on_level_up'))
        except:
            pass

        results[stat_name] = stat_data
        print(f"OK: {stat_name}")

    # Write results to file
    output_path = "C:/scripts/SLFConversion/stat_defaults.json"
    with open(output_path, "w") as f:
        json.dump(results, f, indent=2, default=str)

    print(f"\nExtracted {len(results)} stat defaults to {output_path}")

    return results


if __name__ == "__main__":
    extract_stat_defaults()
