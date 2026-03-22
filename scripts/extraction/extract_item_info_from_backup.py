# extract_item_info_from_backup.py
# Extracts ItemInformation struct data from BACKUP PDA_Item assets
# The backup at C:/scripts/bp_only/ contains the original Blueprint data
# Run in Unreal pointing to the backup project or copy backup items temporarily

import unreal
import json
import re
import os

OUTPUT_FILE = "C:/scripts/SLFConversion/item_info_backup_data.json"

# First, let's read from the JSON exports which were made from the original blueprints
EXPORT_PATH = "C:/scripts/SLFConversion/Exports/BlueprintDNA_v2/DataAsset"

def extract_from_json_exports():
    """
    Extract ItemInformation data from the JSON exports of the original blueprints.
    These were exported before the C++ migration and should have the correct data.
    """
    print("")
    print("=" * 60)
    print("EXTRACTING ITEM INFO FROM JSON EXPORTS")
    print("=" * 60)

    item_data = {}

    if not os.path.exists(EXPORT_PATH):
        print(f"ERROR: Export path not found: {EXPORT_PATH}")
        return item_data

    for filename in os.listdir(EXPORT_PATH):
        if not filename.startswith("DA_") or not filename.endswith(".json"):
            continue

        asset_name = filename[:-5]  # Remove .json
        json_path = os.path.join(EXPORT_PATH, filename)

        print(f"\nProcessing: {asset_name}")

        try:
            with open(json_path, 'r', encoding='utf-8') as f:
                json_data = json.load(f)

            # Look for ItemInformation property in the JSON
            # The JSON structure varies, let's dump it to understand
            print(f"  Keys: {list(json_data.keys())}")

            item_data[asset_name] = {
                'json_path': json_path,
                'json_data': json_data
            }

        except Exception as e:
            print(f"  [ERROR] {e}")

    # Save to JSON file
    print("")
    print("=" * 60)
    print(f"SAVING TO {OUTPUT_FILE}")
    print("=" * 60)

    with open(OUTPUT_FILE, 'w', encoding='utf-8') as f:
        json.dump(item_data, f, indent=2, ensure_ascii=False)

    print(f"\nExtracted {len(item_data)} items")
    return item_data


if __name__ == "__main__":
    extract_from_json_exports()
