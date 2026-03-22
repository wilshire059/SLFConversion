"""
Clear and reparent W_Settings_CenteredText to C++ UW_Settings_CenteredText
"""
import unreal

def main():
    unreal.log_warning("=" * 60)
    unreal.log_warning("FIXING W_Settings_CenteredText")
    unreal.log_warning("=" * 60)

    bp_path = "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_CenteredText"
    cpp_class_path = "/Script/SLFConversion.W_Settings_CenteredText"

    # Load blueprint
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_error(f"Failed to load: {bp_path}")
        return

    # Load C++ class
    cpp_class = unreal.load_class(None, cpp_class_path)
    if not cpp_class:
        unreal.log_error(f"Failed to load C++ class: {cpp_class_path}")
        return

    unreal.log_warning(f"Blueprint: {bp.get_name()}")
    unreal.log_warning(f"C++ Class: {cpp_class.get_name()}")

    # First, try to clear logic using SLFAutomationLibrary
    try:
        result = unreal.SLFAutomationLibrary.clear_all_blueprint_logic(bp, True)  # keep variables
        unreal.log_warning(f"Clear logic result: {result}")
    except Exception as e:
        unreal.log_warning(f"Could not clear logic: {e}")

    # Try reparenting
    try:
        result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, cpp_class)
        if result:
            unreal.log_warning("Reparent SUCCESS!")
            unreal.EditorAssetLibrary.save_asset(bp_path)
            unreal.log_warning(f"Saved: {bp_path}")
        else:
            unreal.log_error("Reparent FAILED!")

            # Get more details about current parent
            gen_class = bp.generated_class()
            if gen_class:
                unreal.log_warning(f"Generated class: {gen_class.get_name()}")

                # Check parent chain
                if hasattr(gen_class, 'get_super_struct'):
                    parent = gen_class.get_super_struct()
                    if parent:
                        unreal.log_warning(f"Current parent: {parent.get_name()}")
                        unreal.log_warning(f"Current parent path: {parent.get_path_name()}")
    except Exception as e:
        unreal.log_error(f"Exception during reparent: {e}")

    unreal.log_warning("=" * 60)

main()
