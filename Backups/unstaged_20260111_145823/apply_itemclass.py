# apply_itemclass.py
# Applies ItemClass using load_class approach

import unreal
import json

input_path = "C:/scripts/SLFConversion/migration_cache/equipment_data.json"

with open(input_path, 'r', encoding='utf-8') as f:
    data = json.load(f)

items = data.get("items", [])
unreal.log_warning(f"Found {len(items)} items")

for item in items:
    asset_path = item.get("path", "")
    item_class_path = item.get("item_class", "")

    if not asset_path or not item_class_path or item_class_path == "":
        continue

    unreal.log_warning(f"Processing: {asset_path}")

    # Load the data asset
    asset = unreal.load_asset(asset_path)
    if asset is None:
        continue

    # Try to load the class
    try:
        loaded_class = unreal.load_class(None, item_class_path)
        if loaded_class:
            unreal.log_warning(f"  Loaded class: {loaded_class}")
            # Now try to set it
            item_info = asset.get_editor_property('item_information')
            # Direct property access
            current = item_info.get_editor_property('item_class')
            unreal.log_warning(f"  Current ItemClass type: {type(current)}")
    except Exception as e:
        unreal.log_error(f"  Failed: {e}")
