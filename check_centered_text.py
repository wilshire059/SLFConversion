"""
Verify W_Settings_CenteredText is reparented to C++
"""
import unreal

def main():
    unreal.log_warning("=" * 60)
    unreal.log_warning("Checking W_Settings_CenteredText Reparenting")
    unreal.log_warning("=" * 60)

    # Check C++ class exists
    cpp_class = unreal.load_class(None, "/Script/SLFConversion.W_Settings_CenteredText")
    if cpp_class:
        unreal.log_warning(f"C++ class exists: {cpp_class.get_name()}")
    else:
        unreal.log_error("C++ class NOT FOUND!")
        return

    # Check Blueprint
    bp = unreal.load_asset("/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_CenteredText")
    if not bp:
        unreal.log_error("Blueprint NOT FOUND!")
        return

    gen_class = bp.generated_class()
    if gen_class:
        unreal.log_warning(f"Blueprint generated class: {gen_class.get_name()}")

        # Check CDO for C++ properties
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            try:
                text_widget = cdo.get_editor_property('text')
                unreal.log_warning(f"C++ property 'text': {text_widget} (exists = REPARENTED)")
            except Exception as e:
                unreal.log_warning(f"C++ property 'text' error: {e}")
                unreal.log_warning(">>> NOT REPARENTED - need to run migration <<<")

    unreal.log_warning("=" * 60)

main()
