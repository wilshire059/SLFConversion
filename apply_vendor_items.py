"""
Apply vendor items to DA_ExampleVendor.
Run this on SLFConversion AFTER the Blueprint has been reparented to UPDA_Vendor.

The vendor should have:
1. DA_ThrowingKnife - infinite stock, price 50
2. DA_Apple - infinite stock, price 100
"""
import unreal

def apply_vendor_items():
    unreal.log_warning("=" * 60)
    unreal.log_warning("APPLYING VENDOR ITEMS TO DA_ExampleVendor")
    unreal.log_warning("=" * 60)

    # Load the vendor data asset
    vendor_path = "/Game/SoulslikeFramework/Data/Vendor/DA_ExampleVendor"
    vendor_asset = unreal.load_asset(vendor_path)

    if not vendor_asset:
        unreal.log_warning("ERROR: Could not load DA_ExampleVendor")
        return

    unreal.log_warning(f"Loaded: {vendor_asset.get_name()}")
    unreal.log_warning(f"Class: {vendor_asset.get_class().get_name()}")

    # Load the item assets
    knife_path = "/Game/SoulslikeFramework/Data/Items/DA_ThrowingKnife"
    apple_path = "/Game/SoulslikeFramework/Data/Items/DA_Apple"

    knife_asset = unreal.load_asset(knife_path)
    apple_asset = unreal.load_asset(apple_path)

    if not knife_asset:
        unreal.log_warning(f"ERROR: Could not load {knife_path}")
        return
    if not apple_asset:
        unreal.log_warning(f"ERROR: Could not load {apple_path}")
        return

    unreal.log_warning(f"Loaded knife: {knife_asset.get_name()}")
    unreal.log_warning(f"Loaded apple: {apple_asset.get_name()}")

    try:
        # Get the Items property
        items = vendor_asset.get_editor_property("Items")
        unreal.log_warning(f"Current Items: {items}")
        unreal.log_warning(f"Items type: {type(items)}")

        # Try using the SLFVendorItems struct directly
        # In UE5 Python, structs are exposed with make_ or as types
        try:
            # Check if SLFVendorItems is available as a type
            if hasattr(unreal, 'SLFVendorItems'):
                struct_type = unreal.SLFVendorItems
                unreal.log_warning(f"Found unreal.SLFVendorItems: {struct_type}")
            else:
                unreal.log_warning("unreal.SLFVendorItems not found as attribute")

            # List what looks like struct types
            slf_attrs = [x for x in dir(unreal) if 'SLF' in x or 'Vendor' in x]
            unreal.log_warning(f"SLF/Vendor related unreal attrs: {slf_attrs[:20]}")

        except Exception as e:
            unreal.log_warning(f"Error checking struct type: {e}")

        # Clear and rebuild the Items set
        # In UE5 Python, TSet is exposed as Set
        items.clear()
        unreal.log_warning("Cleared Items set")

        # Create struct instances using the proper method
        # For UE5 Python, we can try creating structs directly
        try:
            # Method 1: Use make_ function if available
            if hasattr(unreal, 'make_slf_vendor_items'):
                knife_item = unreal.make_slf_vendor_items(
                    item=knife_asset,
                    infinite_stock=True,
                    stock_amount=99,
                    price=50
                )
                apple_item = unreal.make_slf_vendor_items(
                    item=apple_asset,
                    infinite_stock=True,
                    stock_amount=99,
                    price=100
                )
            # Method 2: Use SLFVendorItems class directly
            elif hasattr(unreal, 'SLFVendorItems'):
                knife_item = unreal.SLFVendorItems()
                knife_item.item = knife_asset
                knife_item.infinite_stock = True
                knife_item.stock_amount = 99
                knife_item.price = 50

                apple_item = unreal.SLFVendorItems()
                apple_item.item = apple_asset
                apple_item.infinite_stock = True
                apple_item.stock_amount = 99
                apple_item.price = 100
            else:
                # Method 3: Use dict with lowercase snake_case property names
                # UE5 Python uses snake_case for properties
                knife_item = {
                    "item": knife_asset,
                    "infinite_stock": True,  # Note: no 'b' prefix in Python
                    "stock_amount": 99,
                    "price": 50
                }
                apple_item = {
                    "item": apple_asset,
                    "infinite_stock": True,
                    "stock_amount": 99,
                    "price": 100
                }

            items.add(knife_item)
            items.add(apple_item)
            unreal.log_warning(f"Added items to set")

        except Exception as e:
            unreal.log_warning(f"Error creating/adding struct items: {e}")
            import traceback
            unreal.log_warning(traceback.format_exc())

        # Verify
        items_after = vendor_asset.get_editor_property("Items")
        unreal.log_warning(f"Items after modification: {items_after}")
        if hasattr(items_after, '__len__'):
            unreal.log_warning(f"Items count after: {len(items_after)}")

        # Dump the items to see their properties
        for i, item in enumerate(items_after):
            unreal.log_warning(f"  Item {i}: {item}")
            try:
                item_ref = item.get_editor_property("item") if hasattr(item, 'get_editor_property') else getattr(item, 'item', None)
                infinite = item.get_editor_property("infinite_stock") if hasattr(item, 'get_editor_property') else getattr(item, 'infinite_stock', None)
                stock = item.get_editor_property("stock_amount") if hasattr(item, 'get_editor_property') else getattr(item, 'stock_amount', None)
                price = item.get_editor_property("price") if hasattr(item, 'get_editor_property') else getattr(item, 'price', None)
                unreal.log_warning(f"    item={item_ref}, infinite={infinite}, stock={stock}, price={price}")
            except Exception as e:
                unreal.log_warning(f"    Error reading properties: {e}")

        # Save the asset
        unreal.EditorAssetLibrary.save_asset(vendor_path)
        unreal.log_warning("Saved DA_ExampleVendor")

    except Exception as e:
        unreal.log_warning(f"ERROR: {e}")
        import traceback
        unreal.log_warning(traceback.format_exc())

    unreal.log_warning("=" * 60)

# Run
apply_vendor_items()
