"""
Extract stat default values from Blueprint CDOs using export_text().
"""

import unreal
import json
import re

def extract_stat_defaults():
    """Extract StatInfo defaults from all stat Blueprints using export_text."""

    results = {}

    # List of stat Blueprint paths to check
    stat_paths = [
        # Base stat
        "/Game/SoulslikeFramework/Data/Stats/B_Stat",

        # Secondary stats
        "/Game/SoulslikeFramework/Data/Stats/Secondary/B_HP",
        "/Game/SoulslikeFramework/Data/Stats/Secondary/B_FP",
        "/Game/SoulslikeFramework/Data/Stats/Secondary/B_Stamina",

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

        # Use export_text to get raw property data
        try:
            export_text = unreal.EditorAssetLibrary.export_text(bp_path)

            stat_data = {
                "path": bp_path,
                "class_name": gen_class.get_name(),
            }

            # Parse StatInfo from export text
            # Look for patterns like:
            # StatInfo=(Tag="...", DisplayName="...", CurrentValue=100.0, MaxValue=100.0)

            stat_info_match = re.search(r'StatInfo=\((.*?)\)(?=\s*(?:Min|Only|Show|Stat|$))', export_text, re.DOTALL)
            if stat_info_match:
                stat_info_str = stat_info_match.group(1)

                # Extract Tag
                tag_match = re.search(r'Tag=\(TagName="([^"]+)"\)', stat_info_str)
                if tag_match:
                    stat_data["tag"] = tag_match.group(1)

                # Extract DisplayName
                display_match = re.search(r'DisplayName=(?:NSLOCTEXT\([^)]+,"([^"]+)"\)|"([^"]+)")', stat_info_str)
                if display_match:
                    stat_data["display_name"] = display_match.group(1) or display_match.group(2)

                # Try INVTEXT format
                if "display_name" not in stat_data:
                    display_match2 = re.search(r'DisplayName=INVTEXT\("([^"]+)"\)', stat_info_str)
                    if display_match2:
                        stat_data["display_name"] = display_match2.group(1)

                # Extract CurrentValue
                current_match = re.search(r'CurrentValue=([0-9.]+)', stat_info_str)
                if current_match:
                    stat_data["current_value"] = float(current_match.group(1))

                # Extract MaxValue
                max_match = re.search(r'MaxValue=([0-9.]+)', stat_info_str)
                if max_match:
                    stat_data["max_value"] = float(max_match.group(1))

                # Extract booleans
                show_max_match = re.search(r'bShowMaxValue=(True|False)', stat_info_str)
                if show_max_match:
                    stat_data["show_max_value"] = show_max_match.group(1) == "True"

                display_percent_match = re.search(r'bDisplayAsPercent=(True|False)', stat_info_str)
                if display_percent_match:
                    stat_data["display_as_percent"] = display_percent_match.group(1) == "True"

            # Extract MinValue
            min_match = re.search(r'MinValue=([0-9.]+)', export_text)
            if min_match:
                stat_data["min_value"] = float(min_match.group(1))

            # Extract OnlyMaxValueRelevant
            only_max_match = re.search(r'OnlyMaxValueRelevant=(True|False)', export_text)
            if only_max_match:
                stat_data["only_max_value_relevant"] = only_max_match.group(1) == "True"

            # Extract ShowMaxValueOnLevelUp
            show_levelup_match = re.search(r'ShowMaxValueOnLevelUp=(True|False)', export_text)
            if show_levelup_match:
                stat_data["show_max_value_on_level_up"] = show_levelup_match.group(1) == "True"

            results[stat_name] = stat_data
            print(f"OK: {stat_name} - {stat_data.get('display_name', 'NO_NAME')} = {stat_data.get('current_value', '?')}/{stat_data.get('max_value', '?')}")

        except Exception as e:
            print(f"ERROR: {stat_name} - {e}")
            results[stat_name] = {"error": str(e)}

    # Write results to file
    output_path = "C:/scripts/SLFConversion/stat_defaults.json"
    with open(output_path, "w") as f:
        json.dump(results, f, indent=2, default=str)

    print(f"\nExtracted {len(results)} stat defaults to {output_path}")

    return results


if __name__ == "__main__":
    extract_stat_defaults()
