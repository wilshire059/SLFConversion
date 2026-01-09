# extract_item_info_backup.py
# Extracts ItemInformation struct data from the BACKUP project's PDA_Item assets
# Run with: UnrealEditor-Cmd.exe "C:/scripts/bp_only/bp_only.uproject" -run=pythonscript -script="C:/scripts/SLFConversion/extract_item_info_backup.py"

import unreal
import json
import re

OUTPUT_FILE = "C:/scripts/SLFConversion/item_info_backup_data.json"

def extract_item_info():
    """
    Extract ItemInformation struct data from all DA_* item assets in the backup project.
    Uses export_text() to get struct contents and parses for DisplayName, ItemTag, etc.
    """
    print("")
    print("=" * 60)
    print("EXTRACTING ITEM INFO FROM BACKUP PDA_ITEM ASSETS")
    print("=" * 60)

    item_data = {}
    items_path = "/Game/SoulslikeFramework/Data/Items"

    assets = unreal.EditorAssetLibrary.list_assets(items_path, recursive=True, include_folder=False)
    print(f"Found {len(assets)} assets in {items_path}")

    for asset_path in assets:
        asset_name = asset_path.split("/")[-1].split(".")[0]
        if not asset_name.startswith("DA_"):
            continue

        print(f"\n{'='*40}")
        print(f"Processing: {asset_name}")
        print(f"{'='*40}")

        asset = unreal.EditorAssetLibrary.load_asset(asset_path)
        if not asset:
            print(f"  [SKIP] Could not load")
            continue

        asset_class = asset.get_class()
        print(f"  Class: {asset_class.get_name()}")

        try:
            # Get ItemInformation struct using get_editor_property
            item_info = asset.get_editor_property('ItemInformation')
            if item_info is None:
                print(f"  [SKIP] No ItemInformation property")
                continue

            print(f"  ItemInformation type: {type(item_info)}")

            # Use export_text() to get struct contents
            if hasattr(item_info, 'export_text'):
                export = item_info.export_text()
                print(f"  Export length: {len(export)} chars")
                print(f"  FULL EXPORT TEXT:")
                print(f"  {export}")
            else:
                print(f"  [WARN] No export_text() method, trying direct property access")
                export = str(item_info)

            # Store raw export for debugging
            data = {
                'asset_path': asset_path,
                'class': asset_class.get_name(),
                'export_text': export
            }

            # Try to parse fields from export text
            # DisplayName - FText format: NSLOCTEXT("Namespace", "Key", "Value") or INVTEXT("Value")
            display_matches = [
                re.search(r'DisplayName[^=]*=\s*NSLOCTEXT\([^,]*,\s*[^,]*,\s*"([^"]*)"\)', export),
                re.search(r'DisplayName[^=]*=\s*INVTEXT\("([^"]*)"\)', export),
                re.search(r'DisplayName[^=]*=\s*"([^"]*)"', export),
            ]
            for match in display_matches:
                if match:
                    data['DisplayName'] = match.group(1)
                    print(f"  >>> DisplayName: {data['DisplayName']}")
                    break

            # ItemTag - GameplayTag format
            tag_match = re.search(r'ItemTag[^=]*=\s*\(TagName="([^"]*)"\)', export)
            if tag_match:
                data['ItemTag'] = tag_match.group(1)
                print(f"  >>> ItemTag: {data['ItemTag']}")

            # ItemType
            type_match = re.search(r'ItemType[^=]*=\s*"?(\w+)"?', export)
            if type_match:
                data['ItemType'] = type_match.group(1)
                print(f"  >>> ItemType: {data['ItemType']}")

            # Category
            cat_match = re.search(r'Category=(\w+)', export)
            if cat_match:
                data['Category'] = cat_match.group(1)
                print(f"  >>> Category: {data['Category']}")

            # SubCategory
            subcat_match = re.search(r'SubCategory=(\w+)', export)
            if subcat_match:
                data['SubCategory'] = subcat_match.group(1)
                print(f"  >>> SubCategory: {data['SubCategory']}")

            # ShortDescription
            short_desc_matches = [
                re.search(r'ShortDescription[^=]*=\s*NSLOCTEXT\([^,]*,\s*[^,]*,\s*"([^"]*)"\)', export),
                re.search(r'ShortDescription[^=]*=\s*INVTEXT\("([^"]*)"\)', export),
                re.search(r'ShortDescription[^=]*=\s*"([^"]*)"', export),
            ]
            for match in short_desc_matches:
                if match:
                    data['ShortDescription'] = match.group(1)
                    print(f"  >>> ShortDescription: {data['ShortDescription'][:50]}..." if len(data.get('ShortDescription', '')) > 50 else f"  >>> ShortDescription: {data.get('ShortDescription', '')}")
                    break

            # MaxAmount/StackLimit
            max_match = re.search(r'MaxAmount[^=]*=\s*(\d+)', export)
            if max_match:
                data['MaxAmount'] = int(max_match.group(1))
                print(f"  >>> MaxAmount: {data['MaxAmount']}")

            # bUsable
            usable_match = re.search(r'bUsable[^=]*=\s*(True|False)', export, re.IGNORECASE)
            if usable_match:
                data['bUsable'] = usable_match.group(1).lower() == 'true'
                print(f"  >>> bUsable: {data['bUsable']}")

            # WorldNiagaraSystem
            niagara_match = re.search(r'WorldNiagaraSystem[^=]*=\s*([^\s,\)]+)', export)
            if niagara_match and niagara_match.group(1) != 'None':
                data['WorldNiagaraSystem'] = niagara_match.group(1)
                print(f"  >>> WorldNiagaraSystem: {data['WorldNiagaraSystem']}")

            # WorldStaticMesh
            mesh_match = re.search(r'WorldStaticMesh[^=]*=\s*([^\s,\)]+)', export)
            if mesh_match and mesh_match.group(1) != 'None':
                data['WorldStaticMesh'] = mesh_match.group(1)
                print(f"  >>> WorldStaticMesh: {data['WorldStaticMesh']}")

            # IconSmall
            icon_match = re.search(r'IconSmall[^=]*=\s*([^\s,\)]+)', export)
            if icon_match and icon_match.group(1) != 'None':
                data['IconSmall'] = icon_match.group(1)
                print(f"  >>> IconSmall: {data['IconSmall']}")

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

    # Summary
    print("\n" + "=" * 60)
    print("SUMMARY")
    print("=" * 60)
    for name, data in item_data.items():
        display = data.get('DisplayName', 'N/A')
        tag = data.get('ItemTag', 'N/A')
        print(f"  {name}: DisplayName='{display}', ItemTag='{tag}'")

    return item_data


if __name__ == "__main__":
    extract_item_info()
