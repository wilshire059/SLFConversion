"""
Extract stat default values from Blueprint CDOs using Exporter.
"""

import unreal
import json
import re

def extract_stat_defaults():
    """Extract StatInfo defaults from all stat Blueprints."""

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

        stat_data = {
            "path": bp_path,
            "class_name": gen_class.get_name(),
        }

        # Use Exporter to get raw property data
        try:
            # Create a temporary file path for export
            temp_path = f"C:/scripts/SLFConversion/temp_export_{stat_name}.txt"

            # Create export task
            task = unreal.AssetExportTask()
            task.set_editor_property('automated', True)
            task.set_editor_property('object', cdo)
            task.set_editor_property('filename', temp_path)
            task.set_editor_property('exporter', None)  # Use default exporter
            task.set_editor_property('prompt', False)
            task.set_editor_property('replace_identical', True)
            task.set_editor_property('use_file_archive', False)
            task.set_editor_property('write_empty_files', False)

            # Run the export
            if unreal.Exporter.run_asset_export_task(task):
                # Read the exported file
                with open(temp_path, 'r') as f:
                    export_text = f.read()

                # Parse StatInfo from export text
                stat_info_match = re.search(r'StatInfo=\((.*?)\)(?:\s|$)', export_text, re.DOTALL)
                if stat_info_match:
                    stat_info_str = stat_info_match.group(1)

                    # Extract Tag
                    tag_match = re.search(r'Tag=\(TagName="([^"]+)"\)', stat_info_str)
                    if tag_match:
                        stat_data["tag"] = tag_match.group(1)

                    # Extract DisplayName - try multiple formats
                    display_match = re.search(r'DisplayName=NSLOCTEXT\([^,]+,\s*"[^"]*",\s*"([^"]+)"\)', stat_info_str)
                    if display_match:
                        stat_data["display_name"] = display_match.group(1)
                    else:
                        display_match2 = re.search(r'DisplayName=INVTEXT\("([^"]+)"\)', stat_info_str)
                        if display_match2:
                            stat_data["display_name"] = display_match2.group(1)
                        else:
                            display_match3 = re.search(r'DisplayName="([^"]+)"', stat_info_str)
                            if display_match3:
                                stat_data["display_name"] = display_match3.group(1)

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

                # Clean up temp file
                import os
                try:
                    os.remove(temp_path)
                except:
                    pass

            results[stat_name] = stat_data
            print(f"OK: {stat_name} - {stat_data.get('display_name', 'NO_NAME')} = {stat_data.get('current_value', '?')}/{stat_data.get('max_value', '?')}")

        except Exception as e:
            print(f"ERROR: {stat_name} - {e}")
            results[stat_name] = {"path": bp_path, "error": str(e)}

    # Write results to file
    output_path = "C:/scripts/SLFConversion/stat_defaults.json"
    with open(output_path, "w") as f:
        json.dump(results, f, indent=2, default=str)

    print(f"\nExtracted {len(results)} stat defaults to {output_path}")

    return results


if __name__ == "__main__":
    extract_stat_defaults()
