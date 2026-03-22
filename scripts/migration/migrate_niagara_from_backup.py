# migrate_niagara_from_backup.py
# Reads WorldNiagaraSystem from BACKUP item assets and sets it on CURRENT reparented assets
# Run in Unreal Editor with: -run=pythonscript -script="path/to/this/script.py"

import unreal

# Mapping: item asset name -> Niagara system path
# We'll populate this by reading from backups
NIAGARA_DATA = {}

def extract_niagara_from_backup():
    """
    Read the backup assets and extract WorldNiagaraSystem paths using reflection.
    The backups still have the original Blueprint structure with ItemInformation property.
    """
    print("")
    print("=" * 60)
    print("STEP 1: EXTRACTING NIAGARA DATA FROM BACKUP ASSETS")
    print("=" * 60)

    backup_path = "/Game/SoulslikeFramework/Data/Items"

    # We need to load from the backup folder, not the current project
    # Since UE Python can't directly access external folders during runtime,
    # we'll need a different approach - use AssetRegistry from current project
    # but the backup assets ARE in the current Content folder now

    # List all assets in Items folder
    asset_registry = unreal.AssetRegistryHelpers.get_asset_registry()

    # Get all data assets in the Items folder
    assets = unreal.EditorAssetLibrary.list_assets(backup_path, recursive=True, include_folder=False)

    for asset_path in assets:
        asset_name = asset_path.split("/")[-1].split(".")[0]
        if not asset_name.startswith("DA_"):
            continue

        print(f"\nProcessing: {asset_name}")

        # Load the asset
        asset = unreal.EditorAssetLibrary.load_asset(asset_path)
        if not asset:
            print(f"  [SKIP] Could not load")
            continue

        asset_class = asset.get_class()
        print(f"  Class: {asset_class.get_name()}")

        # Try to access ItemInformation via Python property access
        niagara_path = None

        # Method 1: Direct Python attribute access
        try:
            # Check various possible property name formats
            item_info = None
            for prop_name in ['item_information', 'ItemInformation']:
                if hasattr(asset, prop_name):
                    item_info = getattr(asset, prop_name)
                    print(f"  Found ItemInformation via '{prop_name}'")
                    break

            if item_info is not None:
                # Get WorldPickupInfo
                world_pickup = None
                for prop_name in ['world_pickup_info', 'WorldPickupInfo']:
                    if hasattr(item_info, prop_name):
                        world_pickup = getattr(item_info, prop_name)
                        print(f"  Found WorldPickupInfo via '{prop_name}'")
                        break

                # Also try iterating attributes for GUID-suffixed names
                if world_pickup is None:
                    for attr in dir(item_info):
                        if 'worldpickup' in attr.lower():
                            world_pickup = getattr(item_info, attr)
                            print(f"  Found WorldPickupInfo via '{attr}'")
                            break

                if world_pickup is not None:
                    # Get WorldNiagaraSystem
                    niagara = None
                    for prop_name in ['world_niagara_system', 'WorldNiagaraSystem']:
                        if hasattr(world_pickup, prop_name):
                            niagara = getattr(world_pickup, prop_name)
                            print(f"  Found WorldNiagaraSystem via '{prop_name}'")
                            break

                    # Also try iterating for GUID-suffixed
                    if niagara is None:
                        for attr in dir(world_pickup):
                            if 'niagara' in attr.lower():
                                niagara = getattr(world_pickup, attr)
                                print(f"  Found WorldNiagaraSystem via '{attr}'")
                                break

                    if niagara is not None:
                        # Get the path from the soft reference
                        if hasattr(niagara, 'get_path_name'):
                            niagara_path = niagara.get_path_name()
                        elif hasattr(niagara, 'to_soft_object_path'):
                            soft_path = niagara.to_soft_object_path()
                            niagara_path = str(soft_path) if soft_path else None
                        elif hasattr(niagara, 'get_asset_path'):
                            niagara_path = str(niagara.get_asset_path())
                        else:
                            niagara_path = str(niagara) if niagara else None

                        if niagara_path and niagara_path != "None" and niagara_path != "":
                            print(f"  Niagara Path: {niagara_path}")
                            NIAGARA_DATA[asset_name] = niagara_path
                        else:
                            print(f"  [INFO] No Niagara system set")
                    else:
                        print(f"  [INFO] WorldNiagaraSystem not found in WorldPickupInfo")
                else:
                    print(f"  [INFO] WorldPickupInfo not found in ItemInformation")
            else:
                print(f"  [INFO] ItemInformation not found")

        except Exception as e:
            print(f"  [ERROR] {e}")

    print("")
    print(f"Extracted {len(NIAGARA_DATA)} Niagara paths")
    return NIAGARA_DATA


def apply_niagara_to_cpp_property():
    """
    Apply the extracted Niagara paths to the C++ WorldNiagaraSystem property.
    This works because the assets are now reparented to UPDA_Item which has this property.
    """
    print("")
    print("=" * 60)
    print("STEP 2: APPLYING NIAGARA DATA TO C++ PROPERTIES")
    print("=" * 60)

    if not NIAGARA_DATA:
        print("No Niagara data to apply!")
        return

    items_path = "/Game/SoulslikeFramework/Data/Items"
    assets = unreal.EditorAssetLibrary.list_assets(items_path, recursive=True, include_folder=False)

    applied = 0

    for asset_path in assets:
        asset_name = asset_path.split("/")[-1].split(".")[0]

        if asset_name not in NIAGARA_DATA:
            continue

        niagara_path = NIAGARA_DATA[asset_name]
        print(f"\nApplying to {asset_name}: {niagara_path}")

        # Load the asset
        asset = unreal.EditorAssetLibrary.load_asset(asset_path)
        if not asset:
            print(f"  [ERROR] Could not load asset")
            continue

        # Check if asset has the C++ WorldNiagaraSystem property
        try:
            # Try to set the property
            if hasattr(asset, 'world_niagara_system'):
                # Create soft object reference
                soft_ref = unreal.SoftObjectPath(niagara_path)
                asset.world_niagara_system = soft_ref
                applied += 1
                print(f"  [OK] Set world_niagara_system")
            elif hasattr(asset, 'WorldNiagaraSystem'):
                soft_ref = unreal.SoftObjectPath(niagara_path)
                asset.WorldNiagaraSystem = soft_ref
                applied += 1
                print(f"  [OK] Set WorldNiagaraSystem")
            else:
                print(f"  [ERROR] No C++ WorldNiagaraSystem property found")
                continue

            # Save the asset
            unreal.EditorAssetLibrary.save_asset(asset_path)
            print(f"  [OK] Saved")

        except Exception as e:
            print(f"  [ERROR] {e}")

    print("")
    print(f"Applied Niagara data to {applied} assets")


def main():
    print("")
    print("=" * 60)
    print("NIAGARA DATA MIGRATION FROM BACKUP TO C++")
    print("=" * 60)

    # Step 1: Extract data from backup
    extract_niagara_from_backup()

    # Step 2: Apply to C++ properties
    apply_niagara_to_cpp_property()

    print("")
    print("=" * 60)
    print("MIGRATION COMPLETE")
    print("=" * 60)


if __name__ == "__main__":
    main()
