# extract_icon_paths.py
# Extracts IconSmall and IconLargeOptional paths from item assets using export_text()
# Run in Unreal BEFORE reparenting (on backup content)
#
# Usage:
# 1. Restore backup: powershell -Command "Remove-Item -Path 'C:\scripts\SLFConversion\Content\*' -Recurse -Force; Copy-Item -Path 'C:\scripts\bp_only\Content\*' -Destination 'C:\scripts\SLFConversion\Content\' -Recurse -Force"
# 2. Run: UnrealEditor-Cmd.exe SLFConversion.uproject -run=pythonscript -script="extract_icon_paths.py"
# 3. Then run migration to reparent
# 4. Then run apply_icon_paths.py

import unreal
import json
import re

OUTPUT_FILE = "C:/scripts/SLFConversion/icon_paths.json"

def extract_icon_paths():
    """
    Extract IconSmall and IconLargeOptional paths from all DA_* item assets.
    Uses export_text() to get struct contents and parses for icon paths.
    """
    print("")
    print("=" * 60)
    print("EXTRACTING ICON PATHS FROM ITEM ASSETS")
    print("=" * 60)

    icon_data = {}
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
            if not item_info:
                # Try alternate access via item_information (snake_case)
                try:
                    item_info = asset.get_editor_property('item_information')
                except:
                    pass

            if not item_info:
                print(f"  [SKIP] No ItemInformation property")
                continue

            # Use export_text() to get struct contents
            if not hasattr(item_info, 'export_text'):
                print(f"  [SKIP] No export_text() method")
                continue

            export = item_info.export_text()

            item_icons = {}

            # Parse for IconSmall
            # Format: IconSmall_XX_GUID="/Game/Path/To/Texture.Texture2D"
            match_small = re.search(r'IconSmall[^=]*="([^"]+)"', export)
            if match_small:
                icon_path = match_small.group(1)
                if icon_path and icon_path != "None" and icon_path != "":
                    item_icons['IconSmall'] = icon_path
                    print(f"  [OK] IconSmall: {icon_path}")

            # Parse for IconLargeOptional
            match_large = re.search(r'IconLargeOptional[^=]*="([^"]+)"', export)
            if match_large:
                icon_path = match_large.group(1)
                if icon_path and icon_path != "None" and icon_path != "":
                    item_icons['IconLargeOptional'] = icon_path
                    print(f"  [OK] IconLargeOptional: {icon_path}")

            if item_icons:
                icon_data[asset_name] = item_icons
            else:
                print(f"  [INFO] No icons found in export_text()")

        except Exception as e:
            print(f"  [ERROR] {e}")
            import traceback
            traceback.print_exc()

    # Save to JSON file
    print("")
    print("=" * 60)
    print(f"SAVING TO {OUTPUT_FILE}")
    print("=" * 60)

    with open(OUTPUT_FILE, 'w') as f:
        json.dump(icon_data, f, indent=2)

    print(f"\nExtracted icons for {len(icon_data)} items:")
    for name, icons in icon_data.items():
        print(f"  {name}:")
        for icon_type, path in icons.items():
            print(f"    {icon_type}: {path}")

    return icon_data


if __name__ == "__main__":
    extract_icon_paths()
