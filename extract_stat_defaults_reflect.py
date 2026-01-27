# extract_stat_defaults_reflect.py
# Extract stat CDO defaults from bp_only using property iteration
# Run against bp_only project

import unreal
import json
import os

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/stat_defaults_reflect.json"

# All stat paths to check
STAT_PATHS = {
    # Defense (Damage Negation)
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

    # Attack Power
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

def list_all_properties(obj, name):
    """List all properties of an object for debugging"""
    print(f"\n{name} - Properties:")
    if hasattr(obj, '__dir__'):
        for attr in sorted(dir(obj)):
            if not attr.startswith('_'):
                try:
                    val = getattr(obj, attr)
                    if not callable(val):
                        print(f"  {attr}: {type(val).__name__} = {val}")
                except Exception as e:
                    print(f"  {attr}: ERROR - {e}")

def extract_stat_info(bp_path, name):
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

        print(f"\nProcessing: {name}")

        result = {"name": name, "path": bp_path}

        # Try to get StatInfo using different approaches
        stat_info = None

        # Try get_editor_property with different name variations
        for prop_name in ['stat_info', 'StatInfo', 'statinfo']:
            try:
                stat_info = cdo.get_editor_property(prop_name)
                if stat_info:
                    print(f"  Got StatInfo via get_editor_property('{prop_name}')")
                    break
            except:
                pass

        if stat_info:
            # Extract fields from StatInfo struct
            for field_name in ['current_value', 'CurrentValue', 'max_value', 'MaxValue', 'tag', 'Tag', 'display_name', 'DisplayName']:
                try:
                    val = getattr(stat_info, field_name)
                    snake_name = field_name.lower().replace('value', '_value').replace('name', '_name')
                    result[snake_name] = str(val) if not isinstance(val, (int, float, bool)) else val
                    print(f"    {field_name}: {val}")
                except:
                    pass

            # Try standard field access
            try:
                result["current_value"] = float(stat_info.current_value)
            except:
                pass
            try:
                result["max_value"] = float(stat_info.max_value)
            except:
                pass
            try:
                result["display_name"] = str(stat_info.display_name)
            except:
                pass
            try:
                result["tag"] = str(stat_info.tag)
            except:
                pass
        else:
            print(f"  Could not get StatInfo")
            # List properties for debugging (first stat only)
            if name == "B_DN_Physical":
                list_all_properties(cdo, name)

        return result if "current_value" in result else None

    except Exception as e:
        print(f"Error: {bp_path}: {e}")
        import traceback
        traceback.print_exc()
        return None

def main():
    print("=" * 60)
    print("Extracting stat defaults from bp_only using reflection")
    print("=" * 60)

    results = {}

    for name, path in STAT_PATHS.items():
        result = extract_stat_info(path, name)
        if result:
            results[name] = result

    # Save to JSON
    os.makedirs(os.path.dirname(OUTPUT_FILE), exist_ok=True)
    with open(OUTPUT_FILE, 'w') as f:
        json.dump(results, f, indent=2)

    print("=" * 60)
    print(f"Saved {len(results)} stat defaults to {OUTPUT_FILE}")
    print("=" * 60)

    # Print summary
    if results:
        print("\nSummary of extracted values:")
        for name, data in sorted(results.items()):
            print(f"  {name}: CurrentValue={data.get('current_value', 'N/A')}, MaxValue={data.get('max_value', 'N/A')}")

if __name__ == "__main__":
    main()
