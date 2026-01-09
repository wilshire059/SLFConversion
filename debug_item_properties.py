# debug_item_properties.py
# Debug script to see what properties are available on item assets after reparenting

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/property_debug_output.txt"
OUTPUT_LINES = []

def log(msg):
    """Log message to file and unreal.log_warning for visibility."""
    OUTPUT_LINES.append(str(msg))
    unreal.log_warning(str(msg))

def debug_item_properties():
    """
    Print all available properties on DA_* item assets to see what's accessible
    after reparenting to UPDA_Item.
    """
    log("")
    log("=" * 60)
    log("DEBUGGING ITEM ASSET PROPERTIES")
    log("=" * 60)

    items_path = "/Game/SoulslikeFramework/Data/Items"
    assets = unreal.EditorAssetLibrary.list_assets(items_path, recursive=True, include_folder=False)

    for asset_path in assets:
        asset_name = asset_path.split("/")[-1].split(".")[0]
        if not asset_name.startswith("DA_"):
            continue

        # Just check one item for now
        if asset_name != "DA_HealthFlask":
            continue

        log(f"")
        log(f"{'=' * 60}")
        log(f"ASSET: {asset_name}")
        log(f"{'=' * 60}")

        asset = unreal.EditorAssetLibrary.load_asset(asset_path)
        if not asset:
            log("  Could not load")
            continue

        asset_class = asset.get_class()
        log(f"Class: {asset_class.get_name()}")

        # Try to get item_information struct
        log(f"")
        log(f"--- Testing item_information property access ---")
        try:
            item_info = asset.get_editor_property('item_information')
            log(f"  item_information: {type(item_info)}")

            if item_info:
                # List struct properties using dir
                log(f"")
                log(f"  Properties on item_info (dir):")
                for attr in sorted(dir(item_info)):
                    if not attr.startswith('_'):
                        try:
                            val = getattr(item_info, attr)
                            if callable(val):
                                continue
                            log(f"    {attr}: {type(val).__name__}")
                        except:
                            pass

                # Try various property name formats for usable
                log(f"")
                log(f"  Trying usable property names:")
                for prop_name in ['usable', 'b_usable', 'bUsable', 'Usable']:
                    try:
                        val = item_info.get_editor_property(prop_name)
                        log(f"    {prop_name}: SUCCESS = {val}")
                    except Exception as e:
                        log(f"    {prop_name}: FAILED")

                # Try category
                log(f"")
                log(f"  Trying category property:")
                try:
                    cat = item_info.get_editor_property('category')
                    log(f"    category: {type(cat)}")
                    if cat:
                        log(f"    Category struct properties:")
                        for attr in sorted(dir(cat)):
                            if not attr.startswith('_'):
                                try:
                                    val = getattr(cat, attr)
                                    if callable(val):
                                        continue
                                    log(f"      {attr}: {type(val).__name__} = {val}")
                                except:
                                    pass
                except Exception as e:
                    log(f"    category: FAILED - {e}")

        except Exception as e:
            log(f"  Error getting item_information: {e}")

        break  # Just check one item

    log("")
    log("=" * 60)


if __name__ == "__main__":
    debug_item_properties()
    # Write output to file
    with open(OUTPUT_FILE, 'w') as f:
        f.write('\n'.join(OUTPUT_LINES))
    unreal.log_warning(f"Output written to {OUTPUT_FILE}")
