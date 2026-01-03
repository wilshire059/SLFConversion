# find_enum_references.py
# Parses JSON exports to find all references to E_ValueType enum
# Goal: Match Unreal's 25 asset references and 163 memory references

import os
import json
import re
from collections import defaultdict

# Configuration
JSON_EXPORTS_PATH = r"C:\scripts\SLFConversion_Migration\Backups\SoulsClean\Exports\BlueprintDNA"
ENUM_NAME = "E_ValueType"
ENUM_PATH = "/Game/SoulslikeFramework/Enums/E_ValueType"

# Results storage
asset_references = set()  # Unique assets that reference the enum
memory_references = []     # Individual references (variables, pins, parameters)

def search_dict(obj, path="", asset_name=""):
    """Recursively search a dict/list for enum references."""
    global asset_references, memory_references

    if isinstance(obj, dict):
        for key, value in obj.items():
            current_path = f"{path}.{key}" if path else key

            # Check if this is a type reference to our enum
            if isinstance(value, str):
                if ENUM_NAME in value or ENUM_PATH in value:
                    asset_references.add(asset_name)
                    memory_references.append({
                        "asset": asset_name,
                        "path": current_path,
                        "value": value[:200]  # Truncate long values
                    })

            # Check for enum values (NewEnumerator0, NewEnumerator1)
            if key in ["DefaultValue", "Value"] and isinstance(value, str):
                if value in ["NewEnumerator0", "NewEnumerator1"]:
                    # This might be an enum value, but we need context
                    pass

            # Recurse
            search_dict(value, current_path, asset_name)

    elif isinstance(obj, list):
        for i, item in enumerate(obj):
            search_dict(item, f"{path}[{i}]", asset_name)

def process_json_file(filepath):
    """Process a single JSON file."""
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            data = json.load(f)

        # Extract asset name from the JSON
        asset_name = data.get("AssetName", os.path.basename(filepath))
        asset_path = data.get("AssetPath", filepath)

        search_dict(data, "", asset_path)

    except json.JSONDecodeError as e:
        print(f"  JSON error in {filepath}: {e}")
    except Exception as e:
        print(f"  Error processing {filepath}: {e}")

def main():
    print("=" * 70)
    print(f"  Searching for references to: {ENUM_NAME}")
    print(f"  JSON exports path: {JSON_EXPORTS_PATH}")
    print("=" * 70)

    # Find all JSON files
    json_files = []
    for root, dirs, files in os.walk(JSON_EXPORTS_PATH):
        for file in files:
            if file.endswith('.json'):
                json_files.append(os.path.join(root, file))

    print(f"\n  Found {len(json_files)} JSON files to scan...")

    # Process each file
    for i, filepath in enumerate(json_files):
        if (i + 1) % 50 == 0:
            print(f"  Processing {i + 1}/{len(json_files)}...")
        process_json_file(filepath)

    # Results
    print("\n" + "=" * 70)
    print("  RESULTS")
    print("=" * 70)

    print(f"\n  Asset References: {len(asset_references)}")
    print(f"  Memory References: {len(memory_references)}")

    print("\n  Assets referencing the enum:")
    for asset in sorted(asset_references):
        print(f"    - {asset}")

    # Group memory references by asset
    print("\n  Memory references by asset:")
    refs_by_asset = defaultdict(list)
    for ref in memory_references:
        refs_by_asset[ref["asset"]].append(ref)

    for asset in sorted(refs_by_asset.keys()):
        refs = refs_by_asset[asset]
        print(f"\n    {asset} ({len(refs)} refs):")
        for ref in refs[:5]:  # Show first 5
            print(f"      - {ref['path']}")
        if len(refs) > 5:
            print(f"      ... and {len(refs) - 5} more")

    # Detailed breakdown
    print("\n" + "=" * 70)
    print("  DETAILED BREAKDOWN")
    print("=" * 70)

    # Count by reference type
    var_refs = [r for r in memory_references if "Variables" in r["path"]]
    pin_refs = [r for r in memory_references if "Pin" in r["path"]]
    func_refs = [r for r in memory_references if "Function" in r["path"] or "Parameter" in r["path"]]
    other_refs = [r for r in memory_references if r not in var_refs and r not in pin_refs and r not in func_refs]

    print(f"\n  Variable references: {len(var_refs)}")
    print(f"  Pin references: {len(pin_refs)}")
    print(f"  Function/Parameter references: {len(func_refs)}")
    print(f"  Other references: {len(other_refs)}")

    return asset_references, memory_references

if __name__ == "__main__":
    assets, refs = main()
