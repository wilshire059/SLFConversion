"""
Apply execution tags to DA_Execute_Generic data asset.
The ExecuteFront.Tag was lost during reparenting and needs to be restored.
"""
import unreal

def apply_execution_tags():
    """Apply the correct execution tags to DA_Execute_Generic."""

    asset_path = "/Game/SoulslikeFramework/Data/_AnimationData/Executions/DA_Execute_Generic"

    asset = unreal.EditorAssetLibrary.load_asset(asset_path)
    if not asset:
        print(f"ERROR: Could not load {asset_path}")
        return False

    print(f"Loaded: {asset.get_name()} (class: {asset.get_class().get_name()})")

    # Get ExecuteFront struct
    try:
        exec_front = asset.get_editor_property("execute_front")
        print(f"ExecuteFront: {exec_front}")
        print(f"ExecuteFront type: {type(exec_front)}")

        # Try to get and set tag property
        current_tag = exec_front.get_editor_property("tag")
        print(f"Current tag: {current_tag}, type: {type(current_tag)}")

        # Check what methods/properties GameplayTag has
        print(f"GameplayTag dir: {[x for x in dir(current_tag) if not x.startswith('_')]}")

        # Use import_text to create a tag from string
        front_tag = unreal.GameplayTag()
        result = front_tag.import_text("(TagName=\"SoulslikeFramework.Execution.Front.Generic\")")
        unreal.log_warning(f"import_text result: {result}")

        # Check if tag is valid
        is_valid = unreal.GameplayTagLibrary.is_gameplay_tag_valid(front_tag)
        tag_name = unreal.GameplayTagLibrary.get_tag_name(front_tag)
        unreal.log_warning(f"Created front tag: valid={is_valid}, name={tag_name}")

        # Set the tag
        exec_front.set_editor_property("tag", front_tag)
        unreal.log_warning(f"Set ExecuteFront.Tag")

        # Set struct back
        asset.set_editor_property("execute_front", exec_front)

        # Also ExecuteBack
        exec_back = asset.get_editor_property("execute_back")
        back_tag = unreal.GameplayTag()
        back_tag.import_text("(TagName=\"SoulslikeFramework.Execution.Back.Generic\")")
        exec_back.set_editor_property("tag", back_tag)
        asset.set_editor_property("execute_back", exec_back)
        unreal.log_warning(f"Set ExecuteBack.Tag")

        # Save
        unreal.EditorAssetLibrary.save_asset(asset_path)
        print(f"Saved {asset_path}")

        return True

    except Exception as e:
        print(f"ERROR: {e}")
        import traceback
        traceback.print_exc()
        return False

if __name__ == "__main__":
    apply_execution_tags()
