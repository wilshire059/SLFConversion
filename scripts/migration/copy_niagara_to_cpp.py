# copy_niagara_to_cpp.py
# Copies WorldNiagaraSystem from Blueprint ItemInformation property to C++ property
# Run AFTER run_migration.py has reparented PDA_Item to UPDA_Item

import unreal

def copy_niagara_data():
    """
    For each item data asset (DA_*), copy the WorldNiagaraSystem from:
    - Blueprint property: ItemInformation.WorldPickupInfo.WorldNiagaraSystem
    To:
    - C++ property: WorldNiagaraSystem
    """
    print("")
    print("=" * 60)
    print("COPYING NIAGARA DATA FROM BLUEPRINT TO C++ PROPERTIES")
    print("=" * 60)

    # Find all item data assets
    asset_registry = unreal.AssetRegistryHelpers.get_asset_registry()

    # Search in the Items data folder
    search_paths = [
        "/Game/SoulslikeFramework/Data/Items",
    ]

    items_processed = 0
    items_with_niagara = 0

    for search_path in search_paths:
        # Get all assets in the path
        assets = unreal.EditorAssetLibrary.list_assets(search_path, recursive=True, include_folder=False)

        for asset_path in assets:
            # Only process DA_ prefixed assets
            asset_name = asset_path.split("/")[-1].split(".")[0]
            if not asset_name.startswith("DA_"):
                continue

            # Load the asset
            asset = unreal.EditorAssetLibrary.load_asset(asset_path)
            if not asset:
                print(f"  [SKIP] Could not load: {asset_path}")
                continue

            # Check if it's a PDA_Item derivative
            asset_class = asset.get_class()
            class_name = asset_class.get_name()

            # The asset should now have the C++ WorldNiagaraSystem property
            # Check if property exists
            if not hasattr(asset, 'world_niagara_system'):
                # Try alternate property name formats
                if hasattr(asset, 'WorldNiagaraSystem'):
                    cpp_prop_name = 'WorldNiagaraSystem'
                else:
                    print(f"  [SKIP] {asset_name}: No C++ WorldNiagaraSystem property found")
                    continue
            else:
                cpp_prop_name = 'world_niagara_system'

            items_processed += 1

            # Now we need to read the Blueprint ItemInformation property using reflection
            # This is the tricky part - we need to use unreal's property system

            # Try to get ItemInformation property
            item_info = None
            try:
                # In Python, Blueprint properties may be accessible directly
                if hasattr(asset, 'item_information'):
                    item_info = asset.item_information
                elif hasattr(asset, 'ItemInformation'):
                    item_info = asset.ItemInformation
            except Exception as e:
                print(f"  [SKIP] {asset_name}: Cannot access ItemInformation: {e}")
                continue

            if item_info is None:
                print(f"  [SKIP] {asset_name}: ItemInformation is None")
                continue

            # Get WorldPickupInfo from ItemInformation
            world_pickup_info = None
            try:
                # Try different property name formats (Blueprint properties may have different names)
                for prop_name in ['world_pickup_info', 'WorldPickupInfo', 'WorldPickupInfo_50_300352CD4AE37D50CE2608965ABCE9C4']:
                    if hasattr(item_info, prop_name):
                        world_pickup_info = getattr(item_info, prop_name)
                        break

                # If not found by name, try iterating properties
                if world_pickup_info is None:
                    for attr_name in dir(item_info):
                        if attr_name.lower().startswith('worldpickupinfo'):
                            world_pickup_info = getattr(item_info, attr_name)
                            break
            except Exception as e:
                print(f"  [SKIP] {asset_name}: Cannot access WorldPickupInfo: {e}")
                continue

            if world_pickup_info is None:
                print(f"  [SKIP] {asset_name}: WorldPickupInfo is None")
                continue

            # Get WorldNiagaraSystem from WorldPickupInfo
            niagara_system = None
            try:
                for prop_name in ['world_niagara_system', 'WorldNiagaraSystem', 'WorldNiagaraSystem_14_EF222BEA43E8FE509F34329013D17763']:
                    if hasattr(world_pickup_info, prop_name):
                        niagara_system = getattr(world_pickup_info, prop_name)
                        break

                # If not found by name, try iterating properties
                if niagara_system is None:
                    for attr_name in dir(world_pickup_info):
                        if attr_name.lower().startswith('worldniagarasystem'):
                            niagara_system = getattr(world_pickup_info, attr_name)
                            break
            except Exception as e:
                print(f"  [SKIP] {asset_name}: Cannot access WorldNiagaraSystem from WorldPickupInfo: {e}")
                continue

            if niagara_system is None or (hasattr(niagara_system, 'is_null') and niagara_system.is_null()):
                print(f"  [INFO] {asset_name}: No Niagara system set in Blueprint data")
                continue

            # Copy to C++ property
            try:
                if cpp_prop_name == 'world_niagara_system':
                    asset.world_niagara_system = niagara_system
                else:
                    setattr(asset, cpp_prop_name, niagara_system)

                items_with_niagara += 1
                print(f"  [OK] {asset_name}: Copied Niagara system to C++ property")

                # Mark asset as modified
                unreal.EditorAssetLibrary.save_asset(asset_path)
            except Exception as e:
                print(f"  [ERROR] {asset_name}: Failed to set C++ property: {e}")

    print("")
    print("=" * 60)
    print(f"COMPLETE: Processed {items_processed} items, {items_with_niagara} had Niagara data")
    print("=" * 60)

# Run the copy
copy_niagara_data()
