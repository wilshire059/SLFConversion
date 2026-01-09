# extract_item_info.py
# Extracts ItemInformation struct data from PDA_Item assets using export_text()
# Run in Unreal: "C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" "C:/scripts/SLFConversion/SLFConversion.uproject" -run=pythonscript -script="C:/scripts/SLFConversion/extract_item_info.py"

import unreal
import json
import re

OUTPUT_FILE = "C:/scripts/SLFConversion/item_info_data.json"

def extract_item_info():
    """
    Extract ItemInformation struct data from all DA_* item assets.
    Uses export_text() to get struct contents and parses for DisplayName, ItemTag, etc.
    """
    print("")
    print("=" * 60)
    print("EXTRACTING ITEM INFO FROM PDA_ITEM ASSETS")
    print("=" * 60)

    item_data = {}
    items_path = "/Game/SoulslikeFramework/Data/Items"

    assets = unreal.EditorAssetLibrary.list_assets(items_path, recursive=True, include_folder=False)

    for asset_path in assets:
        asset_name = asset_path.split("/")[-1].split(".")[0]
        if not asset_name.startswith("DA_"):
            continue

        print(f"\nProcessing: {asset_name}")

        asset = unreal.EditorAssetLibrary.load_asset(asset_path)
        if not asset:
            print(f"  [SKIP] Could not load")
            continue

        asset_class = asset.get_class()
        print(f"  Class: {asset_class.get_name()}")

        try:
            # Get ItemInformation struct
            item_info = asset.get_editor_property('ItemInformation')
            if item_info is None:
                print(f"  [SKIP] No ItemInformation property")
                continue

            # Use export_text() to get struct contents
            if not hasattr(item_info, 'export_text'):
                print(f"  [SKIP] No export_text() method")
                continue

            export = item_info.export_text()
            print(f"  Export length: {len(export)} chars")

            # Debug: print first 500 chars
            print(f"  Export preview: {export[:500]}...")

            # Parse fields from export text
            # Format varies but generally: FieldName_XX_GUID=Value or FieldName=Value

            data = {
                'asset_path': asset_path,
                'export_text': export
            }

            # Extract DisplayName - it's FText so might be different format
            # Looking for: DisplayName_XX_GUID=NSLOCTEXT(...) or DisplayName="..."
            display_match = re.search(r'DisplayName[^=]*=\s*(?:NSLOCTEXT\([^)]*,\s*[^)]*,\s*"([^"]*)"\)|"([^"]*)")', export)
            if display_match:
                data['DisplayName'] = display_match.group(1) or display_match.group(2)
                print(f"  DisplayName: {data['DisplayName']}")

            # Extract ItemTag - GameplayTag format
            # Looking for: ItemTag_XX_GUID=(TagName="SoulslikeFramework.Items.Weapon")
            tag_match = re.search(r'ItemTag[^=]*=\s*\(TagName="([^"]+)"\)', export)
            if tag_match:
                data['ItemTag'] = tag_match.group(1)
                print(f"  ItemTag: {data['ItemTag']}")

            # Extract ItemType
            type_match = re.search(r'ItemType[^=]*=\s*(\w+)', export)
            if type_match:
                data['ItemType'] = type_match.group(1)
                print(f"  ItemType: {data['ItemType']}")

            # Extract Description
            desc_match = re.search(r'Description[^=]*=\s*(?:NSLOCTEXT\([^)]*,\s*[^)]*,\s*"([^"]*)"\)|"([^"]*)")', export)
            if desc_match:
                data['Description'] = desc_match.group(1) or desc_match.group(2)
                print(f"  Description: {data.get('Description', '')[:50]}...")

            # Extract StackLimit
            stack_match = re.search(r'StackLimit[^=]*=\s*(\d+)', export)
            if stack_match:
                data['StackLimit'] = int(stack_match.group(1))
                print(f"  StackLimit: {data['StackLimit']}")

            # Extract bCanBeDiscarded
            discard_match = re.search(r'bCanBeDiscarded[^=]*=\s*(True|False)', export, re.IGNORECASE)
            if discard_match:
                data['bCanBeDiscarded'] = discard_match.group(1).lower() == 'true'
                print(f"  bCanBeDiscarded: {data['bCanBeDiscarded']}")

            # Extract Icon path
            icon_match = re.search(r'Icon[^=]*=\s*(?:Texture2D\'|)"?([^"\'\)]+)"?', export)
            if icon_match:
                data['Icon'] = icon_match.group(1)
                print(f"  Icon: {data['Icon']}")

            item_data[asset_name] = data

        except Exception as e:
            print(f"  [ERROR] {e}")
            import traceback
            traceback.print_exc()

    # Save to JSON file
    print("")
    print("=" * 60)
    print(f"SAVING TO {OUTPUT_FILE}")
    print("=" * 60)

    with open(OUTPUT_FILE, 'w', encoding='utf-8') as f:
        json.dump(item_data, f, indent=2, ensure_ascii=False)

    print(f"\nExtracted data for {len(item_data)} items")
    for name, data in item_data.items():
        print(f"  {name}: DisplayName={data.get('DisplayName', 'N/A')}, ItemTag={data.get('ItemTag', 'N/A')}")

    return item_data


if __name__ == "__main__":
    extract_item_info()
