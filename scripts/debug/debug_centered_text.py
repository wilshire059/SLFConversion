"""
Debug W_Settings_CenteredText hierarchy
"""
import unreal

def main():
    unreal.log_warning("=" * 60)
    unreal.log_warning("DEBUG W_Settings_CenteredText")
    unreal.log_warning("=" * 60)

    bp_path = "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_CenteredText"
    cpp_class_path = "/Script/SLFConversion.W_Settings_CenteredText"

    # Load blueprint
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_error(f"Failed to load: {bp_path}")
        return

    unreal.log_warning(f"Blueprint: {bp.get_name()}")
    unreal.log_warning(f"Blueprint type: {bp.get_class().get_name()}")

    # Get generated class
    gen_class = bp.generated_class()
    if gen_class:
        unreal.log_warning(f"Generated class: {gen_class.get_name()}")
        unreal.log_warning(f"Generated class path: {gen_class.get_path_name()}")

        # Get parent via multiple methods
        if hasattr(gen_class, 'get_super_struct'):
            parent = gen_class.get_super_struct()
            if parent:
                unreal.log_warning(f"Parent (get_super_struct): {parent.get_name()}")
                unreal.log_warning(f"Parent path: {parent.get_path_name()}")

    # Check if C++ class exists
    cpp_class = unreal.load_class(None, cpp_class_path)
    if cpp_class:
        unreal.log_warning(f"C++ class exists: {cpp_class.get_name()}")
        unreal.log_warning(f"C++ class path: {cpp_class.get_path_name()}")

        # Check if BP gen_class is child of cpp_class
        if gen_class:
            is_child = gen_class.is_child_of(cpp_class)
            unreal.log_warning(f"BP generated_class is child of C++: {is_child}")
    else:
        unreal.log_error(f"C++ class NOT FOUND: {cpp_class_path}")

    # Try to access C++ specific property
    if gen_class:
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            try:
                text = cdo.get_editor_property('text')
                unreal.log_warning(f"'text' property accessible: {text}")
            except Exception as e:
                unreal.log_warning(f"'text' property NOT accessible: {e}")

    unreal.log_warning("=" * 60)

main()
