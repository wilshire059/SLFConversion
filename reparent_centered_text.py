"""
Reparent W_Settings_CenteredText to C++ UW_Settings_CenteredText
"""
import unreal

def main():
    unreal.log_warning("=" * 60)
    unreal.log_warning("REPARENTING W_Settings_CenteredText to C++")
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

    # Reparent
    try:
        result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, cpp_class)
        if result:
            unreal.log_warning("Reparent SUCCESS!")

            # Save the asset
            unreal.EditorAssetLibrary.save_asset(bp_path)
            unreal.log_warning(f"Saved: {bp_path}")
        else:
            unreal.log_error("Reparent FAILED!")
    except Exception as e:
        unreal.log_error(f"Exception during reparent: {e}")

    unreal.log_warning("=" * 60)

main()
