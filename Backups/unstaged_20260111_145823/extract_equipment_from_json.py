# extract_equipment_from_json.py
# Parses BlueprintDNA JSON exports to extract ItemClass and AttachmentSockets
# No need to run on backup - uses existing JSON exports

import os
import re
import json

# Source: BlueprintDNA exports
json_export_dir = "C:/scripts/SLFConversion/Exports/BlueprintDNA/DataAsset"
output_path = "C:/scripts/SLFConversion/migration_cache/equipment_data.json"

items = []

for filename in os.listdir(json_export_dir):
    if not filename.startswith("DA_") or not filename.endswith(".json"):
        continue

    filepath = os.path.join(json_export_dir, filename)

    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            data = json.load(f)
    except:
        continue

    # Check if this is a PDA_Item asset
    class_path = data.get("ClassPath", "")
    if "PDA_Item" not in class_path:
        continue

    asset_name = data.get("Name", "")
    asset_path = data.get("Path", "")

    # Get ItemInformation property value
    props = data.get("Properties", {})
    item_info = props.get("ItemInformation", {})
    item_info_value = item_info.get("Value", "")

    if not item_info_value:
        continue

    # Parse ItemClass from the value string
    # Format: ItemClass_51_2EF1F34F40CCC0991C32B19CDB795E0C="/Game/..."
    item_class_match = re.search(r'ItemClass_[^=]+=\s*"([^"]*)"', item_info_value)
    item_class = item_class_match.group(1) if item_class_match else ""

    # Parse AttachmentSockets
    # Format: LeftHandSocketName_2_F744F0334FCB7FE2FC6D469C5766C3B8="soulslike_weapon_l"
    left_socket_match = re.search(r'LeftHandSocketName_[^=]+=\s*"([^"]*)"', item_info_value)
    right_socket_match = re.search(r'RightHandSocketName_[^=]+=\s*"([^"]*)"', item_info_value)

    left_socket = left_socket_match.group(1) if left_socket_match else ""
    right_socket = right_socket_match.group(1) if right_socket_match else ""

    # Only include items with meaningful equipment data
    if not item_class and not left_socket and not right_socket:
        continue

    print(f"Found: {asset_name}")
    print(f"  ItemClass: {item_class}")
    print(f"  Sockets: L={left_socket}, R={right_socket}")

    items.append({
        "name": asset_name,
        "path": asset_path,
        "item_class": item_class,
        "left_socket": left_socket,
        "right_socket": right_socket
    })

# Ensure output directory exists
os.makedirs(os.path.dirname(output_path), exist_ok=True)

# Save to JSON
output_data = {"items": items}
with open(output_path, 'w', encoding='utf-8') as f:
    json.dump(output_data, f, indent=2)

print(f"\nExtracted equipment data for {len(items)} items to {output_path}")
