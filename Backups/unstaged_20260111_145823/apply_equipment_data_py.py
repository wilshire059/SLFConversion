# apply_equipment_data_py.py
# Applies ItemClass and AttachmentSockets from JSON cache to PDA_Item data assets
# Run in Unreal Editor on SLFConversion project

import unreal
import json

input_path = "C:/scripts/SLFConversion/migration_cache/equipment_data.json"

# Read JSON file
with open(input_path, 'r', encoding='utf-8') as f:
    data = json.load(f)

items = data.get("items", [])
unreal.log_warning(f"Found {len(items)} items in cache")

success_count = 0
fail_count = 0

for item in items:
    asset_path = item.get("path", "")
    item_class_path = item.get("item_class", "")
    left_socket = item.get("left_socket", "")
    right_socket = item.get("right_socket", "")

    if not asset_path:
        continue

    unreal.log_warning(f"Processing: {asset_path}")

    # Load the asset
    asset = unreal.load_asset(asset_path)
    if asset is None:
        unreal.log_error(f"  Could not load asset")
        fail_count += 1
        continue

    # Check if it has ItemInformation property (UPDA_Item)
    if not hasattr(asset, 'item_information'):
        unreal.log_error(f"  No item_information property")
        fail_count += 1
        continue

    item_info = asset.get_editor_property('item_information')
    modified = False

    # Set ItemClass - Use TopLevelAssetPath for soft class pointer
    if item_class_path and item_class_path != "None" and item_class_path != "":
        try:
            # For TSoftClassPtr, we need to use TopLevelAssetPath
            # Path format: /Game/Path/To/Asset.Asset_C
            soft_class = unreal.TopLevelAssetPath(item_class_path)
            item_info.set_editor_property('item_class', soft_class)
            modified = True
            unreal.log_warning(f"  Set ItemClass: {item_class_path}")
        except Exception as e:
            # Try alternate method - direct path string
            try:
                # Some versions want the path without _C suffix
                item_info.item_class = unreal.SoftClassPath(item_class_path)
                modified = True
                unreal.log_warning(f"  Set ItemClass (alt): {item_class_path}")
            except Exception as e2:
                unreal.log_error(f"  ERROR setting ItemClass: {e} / {e2}")

    # Set socket names in equipment_details.attachment_sockets
    if left_socket or right_socket:
        try:
            equip_details = item_info.get_editor_property('equipment_details')
            sockets = equip_details.get_editor_property('attachment_sockets')

            if left_socket:
                sockets.set_editor_property('left_hand_socket_name', left_socket)
                modified = True
                unreal.log_warning(f"  Set LeftHandSocketName: {left_socket}")

            if right_socket:
                sockets.set_editor_property('right_hand_socket_name', right_socket)
                modified = True
                unreal.log_warning(f"  Set RightHandSocketName: {right_socket}")

            equip_details.set_editor_property('attachment_sockets', sockets)
            item_info.set_editor_property('equipment_details', equip_details)
        except Exception as e:
            unreal.log_error(f"  ERROR setting sockets: {e}")

    # Set the modified item_information back
    if modified:
        try:
            asset.set_editor_property('item_information', item_info)
            # Save the asset
            unreal.EditorAssetLibrary.save_asset(asset_path)
            success_count += 1
            unreal.log_warning(f"  Saved!")
        except Exception as e:
            unreal.log_error(f"  ERROR saving: {e}")
            fail_count += 1
    else:
        unreal.log_warning(f"  No changes made")

unreal.log_warning(f"Applied equipment data: {success_count} succeeded, {fail_count} failed")
