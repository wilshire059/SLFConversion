# apply_item_info_data.py
# Applies ItemInformation data from backup extraction to the migrated C++ assets
# Run with: UnrealEditor-Cmd.exe "C:/scripts/SLFConversion/SLFConversion.uproject" -run=pythonscript -script="C:/scripts/SLFConversion/apply_item_info_data.py"

import unreal
import json

INPUT_FILE = "C:/scripts/SLFConversion/item_info_backup_data.json"

def apply_item_info():
    """
    Apply extracted ItemInformation data to the C++ UPDA_Item assets.
    This migrates DisplayName, ItemTag, and other fields from the backup.
    """
    print("")
    print("=" * 60)
    print("APPLYING ITEM INFO DATA TO MIGRATED ASSETS")
    print("=" * 60)

    # Load extracted data
    with open(INPUT_FILE, 'r', encoding='utf-8') as f:
        item_data = json.load(f)

    print(f"Loaded data for {len(item_data)} items")

    items_path = "/Game/SoulslikeFramework/Data/Items"
    assets = unreal.EditorAssetLibrary.list_assets(items_path, recursive=True, include_folder=False)

    modified_count = 0
    for asset_path in assets:
        asset_name = asset_path.split("/")[-1].split(".")[0]
        if not asset_name.startswith("DA_"):
            continue

        if asset_name not in item_data:
            print(f"[SKIP] {asset_name} - No data in backup")
            continue

        data = item_data[asset_name]
        print(f"\n{'='*40}")
        print(f"Applying data to: {asset_name}")
        print(f"{'='*40}")

        asset = unreal.EditorAssetLibrary.load_asset(asset_path)
        if not asset:
            print(f"  [ERROR] Could not load asset")
            continue

        try:
            # Get the C++ ItemInformation struct
            item_info = asset.get_editor_property('ItemInformation')
            if item_info is None:
                print(f"  [ERROR] No ItemInformation property")
                continue

            print(f"  ItemInformation type: {type(item_info)}")

            modified = False

            # Apply DisplayName
            if 'DisplayName' in data and data['DisplayName']:
                display_name = data['DisplayName'].replace("\\'", "'")  # Unescape
                item_info.set_editor_property('DisplayName', unreal.Text(display_name))
                print(f"  Set DisplayName: {display_name}")
                modified = True

            # Apply ItemTag - use GameplayTagLibrary
            if 'ItemTag' in data and data['ItemTag']:
                tag_name = data['ItemTag']
                try:
                    # Try using BlueprintGameplayTagLibrary
                    tag = unreal.BlueprintGameplayTagLibrary.make_gameplay_tag_from_request_gameplay_tag(unreal.Name(tag_name))
                    item_info.set_editor_property('ItemTag', tag)
                    print(f"  Set ItemTag: {tag_name}")
                    modified = True
                except Exception as e:
                    print(f"  [SKIP ItemTag] {e}")

            # Apply ShortDescription
            if 'ShortDescription' in data and data['ShortDescription']:
                short_desc = data['ShortDescription'].replace("\\'", "'").replace("\\\\", "\\")
                item_info.set_editor_property('ShortDescription', unreal.Text(short_desc))
                print(f"  Set ShortDescription: {short_desc[:50]}...")
                modified = True

            # Apply MaxAmount
            if 'MaxAmount' in data:
                item_info.set_editor_property('MaxAmount', data['MaxAmount'])
                print(f"  Set MaxAmount: {data['MaxAmount']}")
                modified = True

            # Set the modified struct back
            asset.set_editor_property('ItemInformation', item_info)

            if modified:
                # Mark dirty and save
                unreal.EditorAssetLibrary.save_asset(asset_path, only_if_is_dirty=False)
                print(f"  [SAVED] {asset_name}")
                modified_count += 1

        except Exception as e:
            print(f"  [ERROR] {e}")
            import traceback
            traceback.print_exc()

    print("")
    print("=" * 60)
    print(f"COMPLETED - Modified {modified_count} assets")
    print("=" * 60)


if __name__ == "__main__":
    apply_item_info()
