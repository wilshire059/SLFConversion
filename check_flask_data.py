# check_flask_data.py
# Diagnostic script to check current state of DA_Action_DrinkFlask_HP

import unreal

def check_flask_data():
    """Check the flask data asset"""

    da_path = "/Game/SoulslikeFramework/Data/Actions/ActionData/DA_Action_DrinkFlask_HP"

    # Load the data asset
    da_asset = unreal.EditorAssetLibrary.load_asset(da_path)
    if not da_asset:
        print(f"ERROR: Could not load {da_path}")
        return False

    print(f"=== Flask Data Asset Diagnostic ===")
    print(f"Asset: {da_asset.get_name()}")
    print(f"Class: {da_asset.get_class().get_name()}")
    print(f"Path: {da_path}")
    print()

    # List all properties
    print("=== Properties ===")
    for prop_name in dir(da_asset):
        if not prop_name.startswith('_'):
            try:
                # Try to get the property
                val = getattr(da_asset, prop_name)
                if not callable(val):
                    print(f"  {prop_name}: {val}")
            except:
                pass

    print()
    print("=== Editor Properties ===")

    # Try to get RelevantData
    try:
        relevant_data = da_asset.get_editor_property('relevant_data')
        print(f"relevant_data: {relevant_data}")
        print(f"relevant_data type: {type(relevant_data)}")

        # If it's an InstancedStruct, try to extract info
        if hasattr(relevant_data, 'get_script_struct'):
            try:
                struct_type = relevant_data.get_script_struct()
                print(f"Script struct: {struct_type}")
            except Exception as e:
                print(f"get_script_struct error: {e}")

        # Try to get inner struct values
        if hasattr(relevant_data, 'stat_changes_percent'):
            stat_changes = relevant_data.stat_changes_percent
            print(f"stat_changes_percent: {stat_changes}")
        if hasattr(relevant_data, 'drinking_montage'):
            montage = relevant_data.drinking_montage
            print(f"drinking_montage: {montage}")
        if hasattr(relevant_data, 'vfx'):
            vfx = relevant_data.vfx
            print(f"vfx: {vfx}")

    except Exception as e:
        print(f"Could not get relevant_data: {e}")

    # Try action_class
    try:
        action_class = da_asset.get_editor_property('action_class')
        print(f"action_class: {action_class}")
    except Exception as e:
        print(f"Could not get action_class: {e}")

    # Export to text to see full data
    print()
    print("=== Attempting export_text ===")
    try:
        text = unreal.EditorAssetLibrary.get_metadata_tag(da_path, "RelevantData")
        print(f"Metadata RelevantData: {text}")
    except Exception as e:
        print(f"get_metadata_tag error: {e}")

    return True

if __name__ == "__main__":
    check_flask_data()
