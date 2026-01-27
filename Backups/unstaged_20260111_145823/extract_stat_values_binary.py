"""
Extract stat default values from Blueprint .uasset files by reading binary.
Works on backup project without C++ code.

Run on backup:
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/bp_only/bp_only.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/extract_stat_values_binary.py" ^
  -stdout -unattended -nosplash
"""

import unreal
import os
import struct
import json

OUTPUT_PATH = "C:/scripts/SLFConversion/migration_cache/stat_defaults.json"

# Stat Blueprint paths
STAT_PATHS = {
    # Primary stats
    "B_Vigor": "/Game/SoulslikeFramework/Data/Stats/Primary/B_Vigor",
    "B_Strength": "/Game/SoulslikeFramework/Data/Stats/Primary/B_Strength",
    "B_Dexterity": "/Game/SoulslikeFramework/Data/Stats/Primary/B_Dexterity",
    "B_Intelligence": "/Game/SoulslikeFramework/Data/Stats/Primary/B_Intelligence",
    "B_Faith": "/Game/SoulslikeFramework/Data/Stats/Primary/B_Faith",
    "B_Arcane": "/Game/SoulslikeFramework/Data/Stats/Primary/B_Arcane",
    "B_Mind": "/Game/SoulslikeFramework/Data/Stats/Primary/B_Mind",
    "B_Endurance": "/Game/SoulslikeFramework/Data/Stats/Primary/B_Endurance",
    # Secondary stats
    "B_HP": "/Game/SoulslikeFramework/Data/Stats/Secondary/B_HP",
    "B_Stamina": "/Game/SoulslikeFramework/Data/Stats/Secondary/B_Stamina",
    "B_FP": "/Game/SoulslikeFramework/Data/Stats/Secondary/B_FP",
    "B_Poise": "/Game/SoulslikeFramework/Data/Stats/Secondary/B_Poise",
    "B_EquipLoad": "/Game/SoulslikeFramework/Data/Stats/Secondary/B_EquipLoad",
    # Misc stats
    "B_Souls": "/Game/SoulslikeFramework/Data/Stats/Misc/B_Souls",
    "B_Level": "/Game/SoulslikeFramework/Data/Stats/Misc/B_Level",
}

def log(msg):
    unreal.log_warning(str(msg))

def extract_stat_defaults():
    """Extract default values from stat Blueprints using property reflection."""

    log("=" * 60)
    log("EXTRACTING STAT DEFAULTS FROM BLUEPRINTS")
    log("=" * 60)

    os.makedirs(os.path.dirname(OUTPUT_PATH), exist_ok=True)

    all_stats = {}

    for stat_name, stat_path in STAT_PATHS.items():
        log(f"Processing: {stat_name}")

        # Load the Blueprint
        bp = unreal.EditorAssetLibrary.load_asset(stat_path)
        if not bp:
            log(f"  Could not load Blueprint")
            continue

        # Get the generated class
        try:
            gen_class = bp.get_editor_property('generated_class')
            if not gen_class:
                log(f"  No generated class")
                continue
        except Exception as e:
            log(f"  Error getting generated_class: {e}")
            continue

        # Get the CDO (Class Default Object)
        cdo = gen_class.get_default_object()
        if not cdo:
            log(f"  No CDO")
            continue

        # Try to get StatInfo struct
        stat_info = {}

        # Try various property names
        property_names = ['StatInfo', 'stat_info', 'Stat Info']

        for prop_name in property_names:
            try:
                info = cdo.get_editor_property(prop_name.lower().replace(' ', '_'))
                if info:
                    # Extract struct fields
                    try:
                        stat_info['current_value'] = info.get_editor_property('current_value')
                    except:
                        pass
                    try:
                        stat_info['max_value'] = info.get_editor_property('max_value')
                    except:
                        pass
                    try:
                        stat_info['display_name'] = str(info.get_editor_property('display_name'))
                    except:
                        pass
                    try:
                        tag = info.get_editor_property('tag')
                        stat_info['tag'] = str(tag.tag_name) if tag else ""
                    except:
                        pass
                    break
            except Exception as e:
                continue

        # Also try direct property access on CDO
        try:
            for prop in dir(cdo):
                if 'value' in prop.lower() or 'stat' in prop.lower():
                    try:
                        val = getattr(cdo, prop)
                        if isinstance(val, (int, float)):
                            stat_info[prop] = val
                    except:
                        pass
        except:
            pass

        if stat_info:
            all_stats[stat_name] = stat_info
            log(f"  Found: {stat_info}")
        else:
            log(f"  No stat info found")
            # List available properties for debugging
            try:
                props = [p for p in dir(cdo) if not p.startswith('_')]
                log(f"  Available properties: {props[:20]}...")
            except:
                pass

    # Save to JSON
    with open(OUTPUT_PATH, 'w') as f:
        json.dump(all_stats, f, indent=2, default=str)

    log("=" * 60)
    log(f"Saved {len(all_stats)} stats to {OUTPUT_PATH}")
    log("=" * 60)

if __name__ == "__main__":
    extract_stat_defaults()
