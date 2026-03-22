"""
Apply vendor items to DA_ExampleVendor v2 - with explicit package marking.
"""
import unreal

def apply_vendor_items():
    unreal.log_warning("=" * 60)
    unreal.log_warning("APPLYING VENDOR ITEMS TO DA_ExampleVendor v2")
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
        # Create vendor item structs using the SLFVendorItems class
        knife_item = unreal.SLFVendorItems()
        knife_item.set_editor_property("item", knife_asset)
        knife_item.set_editor_property("infinite_stock", True)
        knife_item.set_editor_property("stock_amount", 99)
        knife_item.set_editor_property("price", 50)
        unreal.log_warning(f"Created knife_item: {knife_item}")

        apple_item = unreal.SLFVendorItems()
        apple_item.set_editor_property("item", apple_asset)
        apple_item.set_editor_property("infinite_stock", True)
        apple_item.set_editor_property("stock_amount", 99)
        apple_item.set_editor_property("price", 100)
        unreal.log_warning(f"Created apple_item: {apple_item}")

        # Try setting the Items property as a list/set directly
        items_list = [knife_item, apple_item]
        unreal.log_warning(f"Items list: {items_list}")

        # Method 1: Use set_editor_property with the list
        try:
            vendor_asset.set_editor_property("Items", items_list)
            unreal.log_warning("Set Items via set_editor_property with list")
        except Exception as e:
            unreal.log_warning(f"set_editor_property(Items, list) failed: {e}")

            # Method 2: Get the existing set and modify it
            try:
                items = vendor_asset.get_editor_property("Items")
                items.clear()
                items.add(knife_item)
                items.add(apple_item)
                unreal.log_warning("Modified Items set directly")
            except Exception as e2:
                unreal.log_warning(f"Direct set modification failed: {e2}")

        # Verify in memory
        items_check = vendor_asset.get_editor_property("Items")
        unreal.log_warning(f"Items in memory: {items_check}")
        unreal.log_warning(f"Items count: {len(items_check)}")
        for i, item in enumerate(items_check):
            unreal.log_warning(f"  Item {i}: {item}")

        # Force mark the package as dirty
        package = vendor_asset.get_outer()
        if package:
            unreal.log_warning(f"Package: {package}")

        # Save using EditorAssetLibrary
        unreal.log_warning("Saving with EditorAssetLibrary...")
        save_result = unreal.EditorAssetLibrary.save_asset(vendor_path, only_if_is_dirty=False)
        unreal.log_warning(f"Save result: {save_result}")

        # Also try saving via ObjectTools
        try:
            unreal.EditorAssetSubsystem().save_asset(vendor_path)
            unreal.log_warning("Also saved via EditorAssetSubsystem")
        except Exception as e:
            unreal.log_warning(f"EditorAssetSubsystem save failed (may not exist): {e}")

        # Verify by reloading
        unreal.log_warning("Reloading to verify...")
        # Force reload the asset
        vendor_asset_reloaded = unreal.EditorAssetLibrary.load_asset(vendor_path)
        items_reloaded = vendor_asset_reloaded.get_editor_property("Items")
        unreal.log_warning(f"Items after reload: count={len(items_reloaded)}")
        for i, item in enumerate(items_reloaded):
            item_ref = item.get_editor_property("item")
            unreal.log_warning(f"  Item {i}: {item_ref.get_name() if item_ref else 'None'}")

    except Exception as e:
        unreal.log_warning(f"ERROR: {e}")
        import traceback
        unreal.log_warning(traceback.format_exc())

    unreal.log_warning("=" * 60)

# Run
apply_vendor_items()
