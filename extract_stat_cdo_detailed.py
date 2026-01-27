"""
Extract detailed stat CDO values from bp_only including FStatInfo struct.
This checks the actual Blueprint CDO values, not just the JSON export.
"""
import unreal
import json
import os

OUTPUT_PATH = "C:/scripts/SLFConversion/migration_cache/stat_cdo_detailed.json"

# Stats to check - particularly the ones showing wrong values
STATS_TO_CHECK = [
    # Attack Power
    "/Game/SoulslikeFramework/Data/Stats/Secondary/AttackPower/B_AP_Physical",
    "/Game/SoulslikeFramework/Data/Stats/Secondary/AttackPower/B_AP_Magic",
    "/Game/SoulslikeFramework/Data/Stats/Secondary/AttackPower/B_AP_Fire",
    # Defense/Negation
    "/Game/SoulslikeFramework/Data/Stats/Defense/B_DN_Physical",
    "/Game/SoulslikeFramework/Data/Stats/Defense/B_DN_Fire",
    "/Game/SoulslikeFramework/Data/Stats/Defense/B_DN_Magic",
    # Resistances
    "/Game/SoulslikeFramework/Data/Stats/Defense/B_Resistance_Focus",
    "/Game/SoulslikeFramework/Data/Stats/Defense/B_Resistance_Immunity",
    "/Game/SoulslikeFramework/Data/Stats/Defense/B_Resistance_Robustness",
    "/Game/SoulslikeFramework/Data/Stats/Defense/B_Resistance_Vitality",
    # Primary stats
    "/Game/SoulslikeFramework/Data/Stats/Primary/B_Vigor",
    "/Game/SoulslikeFramework/Data/Stats/Primary/B_Strength",
    "/Game/SoulslikeFramework/Data/Stats/Primary/B_Intelligence",
]

def extract_stat_info(stat_path):
    """Extract FStatInfo from a stat Blueprint CDO."""
    bp = unreal.EditorAssetLibrary.load_asset(stat_path)
    if not bp:
        print(f"  ERROR: Could not load {stat_path}")
        return None

    gen_class = bp.generated_class()
    if not gen_class:
        print(f"  ERROR: No generated class for {stat_path}")
        return None

    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        print(f"  ERROR: No CDO for {stat_path}")
        return None

    result = {
        "path": stat_path,
        "class_name": cdo.get_class().get_name(),
    }

    # Try to get StatInfo struct
    try:
        stat_info = cdo.get_editor_property('stat_info')
        if stat_info:
            result["stat_info"] = {
                "tag": str(stat_info.get_editor_property('tag')) if hasattr(stat_info, 'tag') else "N/A",
                "display_name": str(stat_info.get_editor_property('display_name')) if hasattr(stat_info, 'display_name') else "N/A",
                "current_value": stat_info.get_editor_property('current_value') if hasattr(stat_info, 'current_value') else 0,
                "max_value": stat_info.get_editor_property('max_value') if hasattr(stat_info, 'max_value') else 0,
                "show_max_value": stat_info.get_editor_property('show_max_value') if hasattr(stat_info, 'show_max_value') else False,
            }
            print(f"  StatInfo: current={result['stat_info']['current_value']}, max={result['stat_info']['max_value']}")
    except Exception as e:
        print(f"  Could not get stat_info: {e}")

    # Get StatBehavior for derivation info
    try:
        stat_behavior = cdo.get_editor_property('stat_behavior')
        if stat_behavior:
            stats_to_affect = stat_behavior.get_editor_property('stats_to_affect')
            if stats_to_affect:
                result["stats_to_affect_count"] = len(stats_to_affect)
                result["affected_stats"] = []
                for tag, affected_data in stats_to_affect.items():
                    result["affected_stats"].append(str(tag))
                print(f"  StatsToAffect: {result.get('stats_to_affect_count', 0)} entries")
    except Exception as e:
        pass  # Not all stats have StatBehavior

    # List all properties for debugging
    result["all_properties"] = {}
    for prop_name in dir(cdo):
        if not prop_name.startswith('_') and not callable(getattr(cdo, prop_name, None)):
            try:
                val = getattr(cdo, prop_name)
                if val is not None:
                    result["all_properties"][prop_name] = str(val)[:100]
            except:
                pass

    return result

def main():
    print("=" * 80)
    print("DETAILED STAT CDO EXTRACTION")
    print("=" * 80)

    results = {}

    for stat_path in STATS_TO_CHECK:
        stat_name = stat_path.split('/')[-1]
        print(f"\n--- {stat_name} ---")

        result = extract_stat_info(stat_path)
        if result:
            results[stat_name] = result

    # Save results
    os.makedirs(os.path.dirname(OUTPUT_PATH), exist_ok=True)
    with open(OUTPUT_PATH, 'w') as f:
        json.dump(results, f, indent=2, default=str)

    print(f"\n\nSaved to: {OUTPUT_PATH}")
    print("=" * 80)

if __name__ == "__main__":
    main()
