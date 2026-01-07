# debug_item_properties.py
# Debug script to see what properties are available on item assets after reparenting

import unreal

def debug_item_properties():
    """
    Print all available properties on DA_* item assets to see what's accessible
    after reparenting to UPDA_Item.
    """
    print("")
    print("=" * 60)
    print("DEBUGGING ITEM ASSET PROPERTIES")
    print("=" * 60)

    items_path = "/Game/SoulslikeFramework/Data/Items"
    assets = unreal.EditorAssetLibrary.list_assets(items_path, recursive=True, include_folder=False)

    for asset_path in assets:
        asset_name = asset_path.split("/")[-1].split(".")[0]
        if not asset_name.startswith("DA_"):
            continue

        # Just check one item for now
        if asset_name != "DA_Lantern":
            continue

        print(f"\n{'=' * 60}")
        print(f"ASSET: {asset_name}")
        print(f"{'=' * 60}")

        asset = unreal.EditorAssetLibrary.load_asset(asset_path)
        if not asset:
            print("  Could not load")
            continue

        asset_class = asset.get_class()
        print(f"\nClass: {asset_class.get_name()}")
        print(f"Parent: {asset_class.get_super_class().get_name() if asset_class.get_super_class() else 'None'}")

        # Print all Python-accessible attributes
        print(f"\n--- Python dir() attributes (excluding __*) ---")
        attrs = [a for a in dir(asset) if not a.startswith('_')]
        for attr in sorted(attrs):
            try:
                val = getattr(asset, attr)
                val_type = type(val).__name__
                if callable(val):
                    continue  # Skip methods
                print(f"  {attr}: {val_type} = {val}")
            except Exception as e:
                print(f"  {attr}: ERROR - {e}")

        # Try to get C++ property specifically
        print(f"\n--- Checking for C++ WorldNiagaraSystem ---")
        for prop_name in ['world_niagara_system', 'WorldNiagaraSystem']:
            if hasattr(asset, prop_name):
                val = getattr(asset, prop_name)
                print(f"  {prop_name} EXISTS: {val}")
            else:
                print(f"  {prop_name} NOT FOUND")

        # Try to get Blueprint property
        print(f"\n--- Checking for Blueprint ItemInformation ---")
        for prop_name in ['item_information', 'ItemInformation']:
            if hasattr(asset, prop_name):
                val = getattr(asset, prop_name)
                print(f"  {prop_name} EXISTS: {val}")
            else:
                print(f"  {prop_name} NOT FOUND")

        break  # Just check one item


if __name__ == "__main__":
    debug_item_properties()
