"""
Fix DA_ExampleVendor - extract Items data and ensure it loads properly
"""
import unreal

def fix_vendor():
    unreal.log_warning("=" * 60)
    unreal.log_warning("FIX DA_ExampleVendor VENDOR DATA")
    unreal.log_warning("=" * 60)

    # Load the vendor data asset
    vendor_path = "/Game/SoulslikeFramework/Data/Vendor/DA_ExampleVendor"
    vendor_asset = unreal.load_asset(vendor_path)

    if not vendor_asset:
        unreal.log_warning("ERROR: Could not load DA_ExampleVendor")
        return

    unreal.log_warning("Loaded: " + vendor_asset.get_name())
    unreal.log_warning("Class: " + vendor_asset.get_class().get_name())

    # Try to access Items property
    try:
        items = vendor_asset.get_editor_property("Items")
        unreal.log_warning("Items type: " + str(type(items)))
        if items:
            unreal.log_warning("Items count: " + str(len(items)))
            for i, item in enumerate(items):
                unreal.log_warning("  Item " + str(i) + ": " + str(item))
                # Try to access struct members
                try:
                    item_ref = item.get_editor_property("Item")
                    stock = item.get_editor_property("StockAmount")
                    price = item.get_editor_property("Price")
                    infinite = item.get_editor_property("bInfiniteStock")
                    unreal.log_warning("    -> Item: " + str(item_ref.get_name() if item_ref else "None"))
                    unreal.log_warning("    -> Stock: " + str(stock) + ", Price: " + str(price) + ", Infinite: " + str(infinite))
                except Exception as e2:
                    unreal.log_warning("    -> Error reading struct: " + str(e2))
        else:
            unreal.log_warning("Items is empty or None")
    except Exception as e:
        unreal.log_warning("ERROR accessing Items: " + str(e))

    # Try DefaultSlotCount
    try:
        slot_count = vendor_asset.get_editor_property("DefaultSlotCount")
        unreal.log_warning("DefaultSlotCount: " + str(slot_count))
    except Exception as e:
        unreal.log_warning("ERROR accessing DefaultSlotCount: " + str(e))

    # Try DefaultSlotsPerRow
    try:
        slots_per_row = vendor_asset.get_editor_property("DefaultSlotsPerRow")
        unreal.log_warning("DefaultSlotsPerRow: " + str(slots_per_row))
    except Exception as e:
        unreal.log_warning("ERROR accessing DefaultSlotsPerRow: " + str(e))

    unreal.log_warning("=" * 60)

# Run
fix_vendor()
