"""
Extract full item data from backup Blueprint assets.
Extracts: Category, bUsable, ActionToTrigger, IconSmall, and other key fields.

Run on bp_only (backup) BEFORE migration:
UnrealEditor-Cmd.exe "C:/scripts/bp_only/SLFConversion.uproject" -run=pythonscript -script="C:/scripts/SLFConversion/extract_item_full_data.py"
"""

import unreal
import json
import os
import re

# Output path for extracted data
OUTPUT_PATH = "C:/scripts/SLFConversion/migration_cache/item_full_data.json"

# Item data asset path
ITEMS_PATH = "/Game/SoulslikeFramework/Data/Items/"

def extract_item_data():
    """Extract comprehensive item data from all item assets."""

    items_data = {}

    # Get all assets in the items folder
    asset_registry = unreal.AssetRegistryHelpers.get_asset_registry()

    # Find all PDA_Item assets
    item_assets = unreal.EditorAssetLibrary.list_assets(ITEMS_PATH, recursive=True, include_folder=False)

    print(f"Found {len(item_assets)} assets in {ITEMS_PATH}")

    for asset_path in item_assets:
        if not asset_path.endswith('_C'):  # Skip _C class assets
            asset_name = asset_path.split('/')[-1].split('.')[0]

            # Load the asset
            asset = unreal.EditorAssetLibrary.load_asset(asset_path)
            if not asset:
                print(f"  Could not load: {asset_path}")
                continue

            # Get export text to extract struct data
            export_text = unreal.EditorAssetLibrary.get_metadata_tag(asset, "")
            if not export_text:
                # Use export_text function
                try:
                    export_options = unreal.AssetExportTask()
                    export_options.object = asset
                    export_options.exporter = None
                    export_options.filename = ""
                    export_options.automated = True
                    export_options.prompt = False
                    export_options.write_empty_files = False

                    # Get text representation
                    export_text = str(asset.export_text()) if hasattr(asset, 'export_text') else ""
                except:
                    export_text = ""

            # Try to get properties using reflection
            item_info = {}

            # Use get_editor_property for common fields if available
            try:
                # Try to access ItemInformation struct
                # Since this is a Blueprint, we need to use export_text to get the values
                pass
            except:
                pass

            # Parse from asset text export
            asset_text = unreal.EditorAssetLibrary.get_metadata_tag(asset, "NativeClass")

            # Store basic info
            items_data[asset_name] = {
                "path": asset_path,
                "class": str(asset.get_class().get_name()) if asset else "Unknown"
            }

            print(f"  Processed: {asset_name}")

    return items_data

def extract_from_text_export():
    """Extract data by parsing the text export of assets."""

    items_data = {}

    # Get all item asset paths
    item_assets = unreal.EditorAssetLibrary.list_assets(ITEMS_PATH, recursive=True, include_folder=False)

    print(f"\nExtracting from {len(item_assets)} item assets...")

    for asset_path in item_assets:
        # Skip class assets
        if '_C' in asset_path:
            continue

        asset_name = asset_path.split('/')[-1].split('.')[0]

        # Load the asset
        asset = unreal.EditorAssetLibrary.load_asset(asset_path)
        if not asset:
            continue

        # Get the full text export using SystemLibrary
        try:
            # Export to temp file and read
            temp_path = f"C:/scripts/SLFConversion/migration_cache/temp_{asset_name}.txt"

            # Use copy export
            export_task = unreal.AssetExportTask()
            export_task.object = asset
            export_task.filename = temp_path
            export_task.automated = True
            export_task.prompt = False
            export_task.exporter = None

            if unreal.Exporter.run_asset_export_task(export_task):
                if os.path.exists(temp_path):
                    with open(temp_path, 'r', encoding='utf-8', errors='ignore') as f:
                        text = f.read()

                    # Parse the text for ItemInformation struct
                    item_info = parse_item_info(text, asset_name)
                    items_data[asset_name] = item_info

                    # Clean up temp file
                    os.remove(temp_path)

                    print(f"  Extracted: {asset_name} - Category: {item_info.get('category', 'None')}")
        except Exception as e:
            print(f"  Error processing {asset_name}: {e}")

    return items_data

def parse_item_info(text, asset_name):
    """Parse ItemInformation struct from export text."""

    info = {
        "name": asset_name,
        "category": "None",
        "sub_category": "Miscellaneous",
        "usable": False,
        "rechargeable": False,
        "action_to_trigger": "",
        "icon_small": "",
        "display_name": "",
        "max_amount": 1
    }

    # Category pattern - look for Category enum value
    # Format: Category_X_GUID=(Category=EnumValue,SubCategory=EnumValue)
    category_pattern = r'Category_\d+_[A-F0-9]+\s*=\s*\(Category\s*=\s*(\w+)'
    match = re.search(category_pattern, text)
    if match:
        info["category"] = match.group(1)

    # Also try simpler pattern
    if info["category"] == "None":
        simple_cat = r'Category\s*=\s*E_ItemCategory::(\w+)'
        match = re.search(simple_cat, text)
        if match:
            info["category"] = match.group(1)

    # Usable pattern
    usable_pattern = r'Usable\?\s*_\d+_[A-F0-9]+\s*=\s*(True|False)'
    match = re.search(usable_pattern, text, re.IGNORECASE)
    if match:
        info["usable"] = match.group(1).lower() == 'true'

    # Rechargeable pattern
    rechargeable_pattern = r'Rechargeable\?\s*_\d+_[A-F0-9]+\s*=\s*(True|False)'
    match = re.search(rechargeable_pattern, text, re.IGNORECASE)
    if match:
        info["rechargeable"] = match.group(1).lower() == 'true'

    # ActionToTrigger pattern
    action_pattern = r'ActionToTrigger_\d+_[A-F0-9]+\s*=\s*\(TagName="([^"]+)"\)'
    match = re.search(action_pattern, text)
    if match:
        info["action_to_trigger"] = match.group(1)

    # IconSmall pattern
    icon_pattern = r'IconSmall_\d+_[A-F0-9]+\s*=\s*([^\s,\)]+)'
    match = re.search(icon_pattern, text)
    if match:
        info["icon_small"] = match.group(1)

    # DisplayName pattern - NSLOCTEXT format
    display_pattern = r'DisplayName_\d+_[A-F0-9]+=NSLOCTEXT\([^,]+,\s*"[^"]*",\s*"([^"]+)"\)'
    match = re.search(display_pattern, text)
    if match:
        info["display_name"] = match.group(1)

    # MaxAmount pattern
    max_pattern = r'MaxAmount_\d+_[A-F0-9]+\s*=\s*(\d+)'
    match = re.search(max_pattern, text)
    if match:
        info["max_amount"] = int(match.group(1))

    return info

def main():
    """Main extraction function."""

    print("=" * 60)
    print("ITEM FULL DATA EXTRACTION")
    print("=" * 60)

    # Ensure output directory exists
    os.makedirs(os.path.dirname(OUTPUT_PATH), exist_ok=True)

    # Extract data
    items_data = extract_from_text_export()

    # Save to JSON
    with open(OUTPUT_PATH, 'w') as f:
        json.dump(items_data, f, indent=2)

    print(f"\n{'=' * 60}")
    print(f"Saved {len(items_data)} items to {OUTPUT_PATH}")
    print("=" * 60)

    # Print summary
    categories = {}
    for name, data in items_data.items():
        cat = data.get("category", "Unknown")
        categories[cat] = categories.get(cat, 0) + 1

    print("\nCategory Summary:")
    for cat, count in sorted(categories.items()):
        print(f"  {cat}: {count}")

if __name__ == "__main__":
    main()
