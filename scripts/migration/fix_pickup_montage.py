import unreal

# Set the pickup montage on DA_Action_PickupItemMontage
da_path = "/Game/SoulslikeFramework/Data/Actions/ActionData/DA_Action_PickupItemMontage"
montage_path = "/Game/SoulslikeFramework/Demo/_Animations/Interaction/ItemPickup/AM_SLF_ItemPickup"

da = unreal.EditorAssetLibrary.load_asset(da_path)
montage = unreal.EditorAssetLibrary.load_asset(montage_path)

if da and montage:
    print(f"DA: {da.get_name()}")
    print(f"Montage: {montage.get_name()}")

    # Set the ActionMontage property as a soft reference
    try:
        # Create soft object path to the montage
        soft_path = unreal.SoftObjectPath(montage_path + "." + montage.get_name())
        da.set_editor_property('action_montage', soft_path)
        print(f"Set action_montage to: {soft_path}")

        # Save the asset
        unreal.EditorAssetLibrary.save_asset(da_path)
        print("Saved successfully!")
    except Exception as e:
        print(f"Error setting action_montage: {e}")

        # Try alternative approach - using the montage directly
        try:
            da.set_editor_property('action_montage', montage)
            print(f"Set action_montage directly to: {montage}")
            unreal.EditorAssetLibrary.save_asset(da_path)
            print("Saved successfully (direct assignment)!")
        except Exception as e2:
            print(f"Error with direct assignment: {e2}")
else:
    if not da:
        print(f"Could not load data asset: {da_path}")
    if not montage:
        print(f"Could not load montage: {montage_path}")

# Verify the change
print("\n=== Verification ===")
da_verify = unreal.EditorAssetLibrary.load_asset(da_path)
if da_verify:
    try:
        action_montage = da_verify.get_editor_property('action_montage')
        print(f"ActionMontage is now: {action_montage}")
    except Exception as e:
        print(f"Error reading action_montage: {e}")
