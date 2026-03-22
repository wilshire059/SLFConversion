import unreal

# Check current state of DA_Action_PickupItemMontage
da_path = "/Game/SoulslikeFramework/Data/Actions/ActionData/DA_Action_PickupItemMontage"

da = unreal.EditorAssetLibrary.load_asset(da_path)

if da:
    unreal.log_warning(f"Asset: {da.get_name()}")
    unreal.log_warning(f"Class: {da.get_class().get_name()}")

    # Check ActionMontage
    try:
        action_montage = da.get_editor_property('action_montage')
        unreal.log_warning(f"ActionMontage: {action_montage}")
    except Exception as e:
        unreal.log_error(f"Error getting ActionMontage: {e}")

    # Check RelevantData
    try:
        relevant_data = da.get_editor_property('relevant_data')
        unreal.log_warning(f"RelevantData: {relevant_data}")
    except Exception as e:
        unreal.log_error(f"Error getting RelevantData: {e}")
else:
    unreal.log_error(f"Could not load: {da_path}")
