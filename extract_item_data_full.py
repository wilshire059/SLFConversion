#!/usr/bin/env python
# extract_item_data_full.py
# Extract full item data from bp_only including ItemTag, DisplayName, CrafingUnlocked, etc.

import unreal
import json
import os

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/item_data_full.json"

def extract_item_data():
    """Extract comprehensive item data from all PDA_Item assets"""

    asset_registry = unreal.AssetRegistryHelpers.get_asset_registry()

    # Get all assets in Items folder
    assets = asset_registry.get_assets_by_path("/Game/SoulslikeFramework/Data/Items", recursive=True)

    unreal.log_warning(f"Found {len(assets)} assets in Items folder")

    item_data = {}

    for asset_data in assets:
        asset = asset_data.get_asset()
        if asset is None:
            continue

        class_name = asset.get_class().get_name()
        if "PDA_Item" not in class_name:
            continue

        asset_name = asset.get_name()
        unreal.log_warning(f"Processing: {asset_name}")

        data = {
            "class": class_name,
        }

        # Extract CrafingUnlocked
        try:
            data["crafing_unlocked"] = asset.get_editor_property("crafing_unlocked")
        except:
            data["crafing_unlocked"] = False

        # Extract SellPrice
        try:
            data["sell_price"] = asset.get_editor_property("sell_price")
        except:
            data["sell_price"] = 0

        # Extract ItemInformation struct
        try:
            item_info = asset.get_editor_property("item_information")
            if item_info:
                # ItemTag
                try:
                    item_tag = item_info.get_editor_property("item_tag")
                    data["item_tag"] = str(item_tag) if item_tag else ""
                except:
                    data["item_tag"] = ""

                # DisplayName
                try:
                    display_name = item_info.get_editor_property("display_name")
                    data["display_name"] = str(display_name) if display_name else ""
                except:
                    data["display_name"] = ""

                # ShortDescription
                try:
                    short_desc = item_info.get_editor_property("short_description")
                    data["short_description"] = str(short_desc) if short_desc else ""
                except:
                    data["short_description"] = ""

                # LongDescription
                try:
                    long_desc = item_info.get_editor_property("long_description")
                    data["long_description"] = str(long_desc) if long_desc else ""
                except:
                    data["long_description"] = ""

                # Category
                try:
                    category = item_info.get_editor_property("category")
                    data["category"] = int(category) if category is not None else 0
                except:
                    data["category"] = 0

                # SubCategory
                try:
                    sub_category = item_info.get_editor_property("sub_category")
                    data["sub_category"] = int(sub_category) if sub_category is not None else 0
                except:
                    data["sub_category"] = 0

                # IconSmall
                try:
                    icon_small = item_info.get_editor_property("icon_small")
                    data["icon_small"] = str(icon_small) if icon_small else ""
                except:
                    data["icon_small"] = ""

                # IconLargeOptional
                try:
                    icon_large = item_info.get_editor_property("icon_large_optional")
                    data["icon_large_optional"] = str(icon_large) if icon_large else ""
                except:
                    data["icon_large_optional"] = ""

                # MaxStackSize
                try:
                    max_stack = item_info.get_editor_property("max_stack_size")
                    data["max_stack_size"] = int(max_stack) if max_stack is not None else 1
                except:
                    data["max_stack_size"] = 1

                # Weight
                try:
                    weight = item_info.get_editor_property("weight")
                    data["weight"] = float(weight) if weight is not None else 0.0
                except:
                    data["weight"] = 0.0

                # CraftingDetails - RequiredItems
                try:
                    crafting_details = item_info.get_editor_property("crafting_details")
                    if crafting_details:
                        required_items = crafting_details.get_editor_property("required_items")
                        if required_items:
                            req_items_list = []
                            for key, value in required_items.items():
                                item_path = str(key) if key else ""
                                amount = int(value) if value else 0
                                req_items_list.append({"item": item_path, "amount": amount})
                            data["required_items"] = req_items_list
                        else:
                            data["required_items"] = []
                    else:
                        data["required_items"] = []
                except Exception as e:
                    data["required_items"] = []
                    unreal.log_warning(f"  Could not get crafting_details: {e}")

        except Exception as e:
            unreal.log_warning(f"  Could not get item_information: {e}")

        item_data[asset_name] = data
        unreal.log_warning(f"  CrafingUnlocked: {data.get('crafing_unlocked', False)}, Tag: {data.get('item_tag', 'N/A')}")

    # Write to file
    os.makedirs(os.path.dirname(OUTPUT_FILE), exist_ok=True)
    with open(OUTPUT_FILE, "w") as f:
        json.dump(item_data, f, indent=2)

    unreal.log_warning(f"Saved {len(item_data)} items to {OUTPUT_FILE}")

if __name__ == "__main__":
    extract_item_data()
