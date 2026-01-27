"""
Verify execution tags were applied to DA_Execute_Generic data asset.
"""
import unreal

def verify_execution_tags():
    """Verify the execution tags on DA_Execute_Generic."""

    asset_path = "/Game/SoulslikeFramework/Data/_AnimationData/Executions/DA_Execute_Generic"

    asset = unreal.EditorAssetLibrary.load_asset(asset_path)
    if not asset:
        unreal.log_error(f"Could not load {asset_path}")
        return False

    unreal.log_warning(f"Loaded: {asset.get_name()} (class: {asset.get_class().get_name()})")

    try:
        # Get ExecuteFront struct
        exec_front = asset.get_editor_property("execute_front")
        front_tag = exec_front.get_editor_property("tag")

        front_valid = unreal.GameplayTagLibrary.is_gameplay_tag_valid(front_tag)
        front_name = unreal.GameplayTagLibrary.get_tag_name(front_tag)
        unreal.log_warning(f"ExecuteFront.Tag: valid={front_valid}, name={front_name}")

        # Get ExecuteBack struct
        exec_back = asset.get_editor_property("execute_back")
        back_tag = exec_back.get_editor_property("tag")

        back_valid = unreal.GameplayTagLibrary.is_gameplay_tag_valid(back_tag)
        back_name = unreal.GameplayTagLibrary.get_tag_name(back_tag)
        unreal.log_warning(f"ExecuteBack.Tag: valid={back_valid}, name={back_name}")

        # Get animations
        front_anim = exec_front.get_editor_property("animation")
        back_anim = exec_back.get_editor_property("animation")
        unreal.log_warning(f"ExecuteFront.Animation: {front_anim}")
        unreal.log_warning(f"ExecuteBack.Animation: {back_anim}")

        if front_valid and back_valid:
            unreal.log_warning("SUCCESS: Execution tags are valid!")
            return True
        else:
            unreal.log_error("FAILED: Some tags are not valid")
            return False

    except Exception as e:
        unreal.log_error(f"ERROR: {e}")
        import traceback
        traceback.print_exc()
        return False

if __name__ == "__main__":
    verify_execution_tags()
