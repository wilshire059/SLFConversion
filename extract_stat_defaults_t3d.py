# extract_stat_defaults_t3d.py
# Extract stat CDO defaults from bp_only using T3D export
# Run against bp_only project: "C:/scripts/bp_only/bp_only.uproject"

import unreal
import json
import os
import re

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/stat_defaults_t3d.json"

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
    "B_Souls": "/Game/SoulslikeFramework/Data/Stats/Misc/B_Souls",

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

def parse_stat_from_text(text, name):
    """Parse stat values from exported text"""
    result = {"name": name}

    # Find CurrentValue in StatInfo struct
    # Pattern: CurrentValue_11_EEB1DDB64CB05B32EFEB3987C7578E44=5.000000
    current_match = re.search(r'CurrentValue[^=]*=([0-9.-]+)', text)
    if current_match:
        result["current_value"] = float(current_match.group(1))
        print(f"  Found CurrentValue: {result['current_value']}")
    else:
        result["current_value"] = 0.0

    # Find MaxValue
    max_match = re.search(r'MaxValue[^=]*=([0-9.-]+)', text)
    if max_match:
        result["max_value"] = float(max_match.group(1))
        print(f"  Found MaxValue: {result['max_value']}")
    else:
        result["max_value"] = 0.0

    # Find DisplayName
    # Pattern: DisplayName_40_...=NSLOCTEXT(...) or ="Physical Defense"
    display_match = re.search(r'DisplayName[^=]*="([^"]+)"', text)
    if display_match:
        result["display_name"] = display_match.group(1)
        print(f"  Found DisplayName: {result['display_name']}")
    else:
        result["display_name"] = name

    # Find Tag
    tag_match = re.search(r'Tag[^=]*=\(TagName="([^"]+)"\)', text)
    if tag_match:
        result["tag"] = tag_match.group(1)
        print(f"  Found Tag: {result['tag']}")
    else:
        result["tag"] = ""

    # Find bShowMaxValue
    show_max_match = re.search(r'ShowMaxValue[^=]*=(True|False)', text)
    if show_max_match:
        result["show_max_value"] = show_max_match.group(1) == "True"
    else:
        result["show_max_value"] = False

    # Find DisplayAsPercent
    percent_match = re.search(r'DisplayasPercent[^=]*=(True|False)', text)
    if percent_match:
        result["display_as_percent"] = percent_match.group(1) == "True"
    else:
        result["display_as_percent"] = False

    return result

def extract_stat_info(bp_path, name):
    """Extract StatInfo from a stat Blueprint using text export"""
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

        print(f"\nExporting: {name}")

        # Export CDO to text
        exporter = unreal.Exporter()
        text = exporter.export_to_clipboard(cdo)

        if not text:
            print(f"  No text export")
            return None

        print(f"  Export length: {len(text)} chars")

        # Save the raw export for debugging
        debug_dir = "C:/scripts/SLFConversion/migration_cache/stat_exports"
        os.makedirs(debug_dir, exist_ok=True)
        with open(f"{debug_dir}/{name}.txt", 'w', encoding='utf-8') as f:
            f.write(text)

        result = parse_stat_from_text(text, name)
        result["path"] = bp_path
        return result

    except Exception as e:
        print(f"Error: {bp_path}: {e}")
        import traceback
        traceback.print_exc()
        return None

def main():
    print("=" * 60)
    print("Extracting stat defaults from bp_only using T3D export")
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
    print("\nSummary of extracted values:")
    for name, data in sorted(results.items()):
        print(f"  {name}: CurrentValue={data.get('current_value', 'N/A')}, MaxValue={data.get('max_value', 'N/A')}")

if __name__ == "__main__":
    main()
