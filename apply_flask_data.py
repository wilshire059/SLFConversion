# apply_flask_data.py
# Restore FSLFFlaskData to DA_Action_DrinkFlask_HP after migration
# This data was lost during reparenting due to InstancedStruct serialization

import unreal

def apply_flask_data():
    """Restore the flask data to DA_Action_DrinkFlask_HP"""

    da_path = "/Game/SoulslikeFramework/Data/Actions/ActionData/DA_Action_DrinkFlask_HP"

    # Load the data asset
    da_asset = unreal.EditorAssetLibrary.load_asset(da_path)
    if not da_asset:
        print(f"ERROR: Could not load {da_path}")
        return False

    print(f"Loaded data asset: {da_asset.get_name()}")
    print(f"Class: {da_asset.get_class().get_name()}")

    # Check if RelevantData exists and what type it is
    try:
        relevant_data = da_asset.get_editor_property('relevant_data')
        print(f"Current RelevantData type: {type(relevant_data)}")

        # If InstancedStruct, try to get its script struct
        if hasattr(relevant_data, 'get_script_struct'):
            struct_type = relevant_data.get_script_struct()
            print(f"InstancedStruct script struct: {struct_type}")
    except Exception as e:
        print(f"Could not get RelevantData: {e}")

    # The bp_only data shows:
    # - StatChangesPercent with HP at 40% (SoulslikeFramework.Stat.Secondary.HP)
    # - DrinkingMontage = AM_SLF_DrinkFlask
    # - VFX = NS_Flask_HP

    # Create the FStatChangePercent struct
    stat_change = unreal.StatChangePercent()

    # Use the correct API for GameplayTag - create from tag name string
    hp_tag = unreal.GameplayTagsManager.request_gameplay_tag(unreal.Name("SoulslikeFramework.Stat.Secondary.HP"))
    stat_change.set_editor_property('stat_tag', hp_tag)
    stat_change.set_editor_property('value_type', unreal.SLFValueType.CURRENT_VALUE)
    stat_change.set_editor_property('percent_change', 40.0)
    stat_change.set_editor_property('try_activate_regen', False)

    print(f"Created StatChangePercent: Percent={stat_change.get_editor_property('percent_change')}")

    # Load the montage and VFX assets
    montage_path = "/Game/SoulslikeFramework/Demo/_Animations/Interaction/Flask/AM_SLF_DrinkFlask"
    vfx_path = "/Game/SoulslikeFramework/VFX/Systems/NS_Flask_HP"

    montage = unreal.EditorAssetLibrary.load_asset(montage_path)
    vfx = unreal.EditorAssetLibrary.load_asset(vfx_path)

    if montage:
        print(f"Loaded montage: {montage.get_name()}")
    else:
        print(f"WARNING: Could not load montage at {montage_path}")

    if vfx:
        print(f"Loaded VFX: {vfx.get_name()}")
    else:
        print(f"WARNING: Could not load VFX at {vfx_path}")

    # Create FSLFFlaskData struct
    try:
        flask_data = unreal.SLFFlaskData()
        flask_data.stat_changes_percent = [stat_change]

        # Set soft object pointers
        if montage:
            flask_data.drinking_montage = unreal.SoftObjectPath(montage_path)
        if vfx:
            flask_data.vfx = unreal.SoftObjectPath(vfx_path)

        print(f"Created FSLFFlaskData with {len(flask_data.stat_changes_percent)} stat changes")

        # Try to set as InstancedStruct
        # This is the tricky part - InstancedStruct in Python
        try:
            instanced_struct = unreal.InstancedStruct()
            instanced_struct.set_struct(flask_data)
            da_asset.set_editor_property('relevant_data', instanced_struct)
            print("Set RelevantData via InstancedStruct.set_struct()")
        except Exception as e:
            print(f"Could not set via InstancedStruct: {e}")
            # Try direct assignment
            try:
                da_asset.set_editor_property('relevant_data', flask_data)
                print("Set RelevantData directly")
            except Exception as e2:
                print(f"Could not set directly either: {e2}")
                return False

        # Save the asset
        if unreal.EditorAssetLibrary.save_asset(da_path):
            print(f"SUCCESS: Saved {da_path}")
            return True
        else:
            print(f"ERROR: Could not save {da_path}")
            return False

    except Exception as e:
        print(f"ERROR creating FSLFFlaskData: {e}")
        import traceback
        traceback.print_exc()
        return False

if __name__ == "__main__":
    success = apply_flask_data()
    print(f"\nResult: {'SUCCESS' if success else 'FAILED'}")
