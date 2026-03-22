import unreal

# Set the ActionMontage on DA_Action_PickupItemMontage
da_path = "/Game/SoulslikeFramework/Data/Actions/ActionData/DA_Action_PickupItemMontage"
montage_path = "/Game/SoulslikeFramework/Demo/_Animations/Interaction/ItemPickup/AM_SLF_ItemPickup"

da = unreal.EditorAssetLibrary.load_asset(da_path)
montage = unreal.EditorAssetLibrary.load_asset(montage_path)

if not da:
    unreal.log_error(f"Could not load data asset: {da_path}")
elif not montage:
    unreal.log_error(f"Could not load montage: {montage_path}")
else:
    unreal.log_warning(f"DA: {da.get_name()}")
    unreal.log_warning(f"Montage: {montage.get_name()}")

    # Try setting the montage directly (for soft object properties, pass the object)
    try:
        da.set_editor_property('action_montage', montage)
        unreal.log_warning(f"Set action_montage directly to montage object")

        # Save the asset
        result = unreal.EditorAssetLibrary.save_asset(da_path, only_if_is_dirty=False)
        unreal.log_warning(f"Save result: {result}")

        # Verify
        verify_da = unreal.EditorAssetLibrary.load_asset(da_path)
        verify_montage = verify_da.get_editor_property('action_montage')
        unreal.log_warning(f"Verification - ActionMontage: {verify_montage}")

    except Exception as e:
        unreal.log_error(f"Error method 1: {e}")

        # Try alternative - use set_editor_properties
        try:
            # Get CDO if this is a Blueprint
            cdo = unreal.get_default_object(da.get_class())
            unreal.log_warning(f"Got CDO: {cdo}")
            cdo.set_editor_property('action_montage', montage)
            unreal.EditorAssetLibrary.save_asset(da_path, only_if_is_dirty=False)
            unreal.log_warning(f"Set via CDO")
        except Exception as e2:
            unreal.log_error(f"Error method 2: {e2}")
