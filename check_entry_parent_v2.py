"""
Check W_Settings_Entry parent class hierarchy
"""
import unreal

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("CHECKING W_SETTINGS_ENTRY PARENT HIERARCHY")
    unreal.log_warning("=" * 70)

    # Load W_Settings_Entry Blueprint
    entry_bp = unreal.load_asset("/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_Entry")
    if not entry_bp:
        unreal.log_error("Failed to load W_Settings_Entry!")
        return

    # Get generated class
    gen_class = entry_bp.generated_class() if hasattr(entry_bp, 'generated_class') else None
    if not gen_class:
        unreal.log_error("No generated_class!")
        return

    unreal.log_warning(f"Generated class: {gen_class.get_name()}")
    unreal.log_warning(f"Full path: {gen_class.get_path_name()}")

    # Try to get parent hierarchy using class reflection
    class_hierarchy = []
    current = gen_class
    for i in range(10):  # max 10 levels
        class_name = current.get_name()
        class_hierarchy.append(class_name)

        # Check if this is a C++ class (no _C suffix at end)
        if not class_name.endswith('_C'):
            unreal.log_warning(f"  -> Found native C++ class: {class_name}")
            break

        # Get the parent class using class methods
        # UClass has a get_class_path_name() we might be able to use
        try:
            # Look at the parent blueprint
            parent_struct = current.get_outer()
            if parent_struct:
                unreal.log_warning(f"  Outer: {parent_struct.get_name()}")
        except:
            pass

        # Try getting CDO's class type chain
        try:
            # Another approach - look for native parent via CDO inspection
            cdo = unreal.get_default_object(current)
            if cdo:
                cdo_class = cdo.get_class()
                cdo_class_name = cdo_class.get_name() if cdo_class else "None"
                unreal.log_warning(f"  CDO class: {cdo_class_name}")
        except Exception as e:
            unreal.log_warning(f"  CDO error: {e}")

        break

    unreal.log_warning(f"\nClass chain: {' -> '.join(class_hierarchy)}")

    # Check the Blueprint's parent class property
    try:
        parent_class = entry_bp.get_editor_property("parent_class")
        if parent_class:
            unreal.log_warning(f"Blueprint ParentClass: {parent_class.get_name()}")
    except Exception as e:
        unreal.log_warning(f"Could not get parent_class property: {e}")

    # Check CDO properties directly
    unreal.log_warning("\n--- CDO Property Check ---")
    try:
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            # Check if DropDown widget is bound (at CDO level, it won't be - but check EntryType etc)
            entry_type = cdo.get_editor_property('entry_type')
            setting_tag = cdo.get_editor_property('setting_tag')
            setting_name = cdo.get_editor_property('setting_name')

            unreal.log_warning(f"  CDO entry_type: {entry_type}")
            unreal.log_warning(f"  CDO setting_tag: {setting_tag}")
            unreal.log_warning(f"  CDO setting_name: {setting_name}")
    except Exception as e:
        unreal.log_warning(f"  CDO property error: {e}")

    unreal.log_warning("=" * 70)

main()
