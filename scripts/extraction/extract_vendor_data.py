"""
Extract vendor item data from bp_only DA_ExampleVendor for migration to C++ UPDA_Vendor.
Run this script on bp_only BEFORE restore to cache the vendor items data.
"""
import unreal
import json
import os

def extract_vendor_data():
    """Extract vendor items from DA_ExampleVendor in bp_only"""

    vendor_path = "/Game/SoulslikeFramework/Data/Vendor/DA_ExampleVendor"
    vendor_asset = unreal.load_asset(vendor_path)

    if not vendor_asset:
        unreal.log_warning("ERROR: Could not load DA_ExampleVendor")
        return

    unreal.log_warning("=" * 60)
    unreal.log_warning("EXTRACTING VENDOR DATA FROM bp_only")
    unreal.log_warning("=" * 60)
    unreal.log_warning(f"Loaded: {vendor_asset.get_name()}")
    unreal.log_warning(f"Class: {vendor_asset.get_class().get_name()}")

    # Use export_text to get all property data
    export_text = unreal.EditorAssetLibrary.export_text(vendor_path)

    vendor_data = {
        "asset_path": vendor_path,
        "items": []
    }

    # Parse the export text to find Items array
    # Blueprint struct property names have GUID suffixes
    import re

    # Look for Items array entries with GUID-suffixed property names
    # Pattern: Item_2_GUID="path", InfiniteStock?_9_GUID=True/False, StockAmount_10_GUID=X, Price_7_GUID=Y

    # Find all item blocks in the export
    # Items is a TSet<FVendorItems>
    lines = export_text.split('\n')

    current_item = None
    in_items_section = False

    for line in lines:
        # Check if we're entering Items section
        if 'Items=' in line or 'Items(' in line:
            in_items_section = True
            unreal.log_warning(f"Found Items section: {line[:100]}")

        # Look for item references (the Item property with GUID suffix)
        item_match = re.search(r'Item_\d+_[A-F0-9]+="([^"]+)"', line)
        if item_match:
            item_path = item_match.group(1)
            # Clean up the path - remove class prefix if present
            if "'" in item_path:
                # Format: ClassName'Path'
                item_path = item_path.split("'")[1] if "'" in item_path else item_path
            current_item = {"item_path": item_path}
            unreal.log_warning(f"Found item: {item_path}")

        # Look for infinite stock
        infinite_match = re.search(r'InfiniteStock\?_\d+_[A-F0-9]+=(True|False)', line, re.IGNORECASE)
        if infinite_match and current_item:
            current_item["infinite_stock"] = infinite_match.group(1).lower() == 'true'

        # Look for stock amount
        stock_match = re.search(r'StockAmount_\d+_[A-F0-9]+=(\d+)', line)
        if stock_match and current_item:
            current_item["stock_amount"] = int(stock_match.group(1))

        # Look for price
        price_match = re.search(r'Price_\d+_[A-F0-9]+=(\d+)', line)
        if price_match and current_item:
            current_item["price"] = int(price_match.group(1))
            # Price is the last property, so we can add the item now
            vendor_data["items"].append(current_item)
            current_item = None

    # Also try using get_editor_property directly on the Items array
    try:
        items_prop = vendor_asset.get_editor_property("Items")
        if items_prop:
            unreal.log_warning(f"Items property type: {type(items_prop)}")
            unreal.log_warning(f"Items count: {len(items_prop)}")

            for i, item in enumerate(items_prop):
                unreal.log_warning(f"  Item {i}: {item}")
                # Try to access struct members - they may have different names in the Blueprint
                try:
                    # Try various property name patterns
                    for prop_name in ["Item", "Item_2_1116033544AEBC89AAD2A6802774460B"]:
                        try:
                            item_ref = item.get_editor_property(prop_name)
                            if item_ref:
                                unreal.log_warning(f"    Found via {prop_name}: {item_ref.get_path_name()}")
                                break
                        except:
                            pass
                except Exception as e:
                    unreal.log_warning(f"    Error reading struct: {e}")
    except Exception as e:
        unreal.log_warning(f"Could not read Items property directly: {e}")

    # If we didn't find items via parsing, try a different approach
    if not vendor_data["items"]:
        unreal.log_warning("No items found via text parsing, checking export text...")
        # Save the full export text for manual inspection
        cache_dir = "C:/scripts/SLFConversion/migration_cache"
        os.makedirs(cache_dir, exist_ok=True)
        export_path = os.path.join(cache_dir, "DA_ExampleVendor_export.txt")
        with open(export_path, 'w', encoding='utf-8') as f:
            f.write(export_text)
        unreal.log_warning(f"Saved export text to: {export_path}")

    # Save vendor data to cache
    cache_dir = "C:/scripts/SLFConversion/migration_cache"
    os.makedirs(cache_dir, exist_ok=True)
    cache_path = os.path.join(cache_dir, "vendor_data.json")

    with open(cache_path, 'w', encoding='utf-8') as f:
        json.dump(vendor_data, f, indent=2)

    unreal.log_warning(f"Saved {len(vendor_data['items'])} items to: {cache_path}")
    unreal.log_warning("=" * 60)

# Run
extract_vendor_data()
