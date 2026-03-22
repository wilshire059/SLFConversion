"""
Check ANS_ToggleChaosField Blueprint parent class.
"""
import unreal

def check_notify():
    bp_path = "/Game/SoulslikeFramework/Blueprints/AnimationRelated/Notifies/ANS_ToggleChaosField"

    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_error(f"Could not load {bp_path}")
        return

    unreal.log_warning(f"=== ANS_ToggleChaosField Status ===")
    unreal.log_warning(f"Blueprint: {bp.get_name()}")

    # Get parent class from Blueprint
    try:
        parent_class = bp.get_editor_property("parent_class")
        if parent_class:
            unreal.log_warning(f"Parent Class: {parent_class.get_name()}")
            unreal.log_warning(f"Parent Path: {parent_class.get_path_name()}")
    except Exception as e:
        unreal.log_error(f"Error getting parent: {e}")

    # Check if C++ class exists by trying to load it
    cpp_class_path = "/Script/SLFConversion.ANS_ToggleChaosField"
    cpp_class = unreal.load_class(None, cpp_class_path)
    if cpp_class:
        unreal.log_warning(f"C++ Class Exists: {cpp_class.get_name()}")
        unreal.log_warning(f"C++ Class Path: {cpp_class.get_path_name()}")
    else:
        unreal.log_error(f"C++ Class NOT FOUND at {cpp_class_path}!")

if __name__ == "__main__":
    check_notify()
