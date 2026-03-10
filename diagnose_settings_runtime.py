"""
Diagnose Settings Widgets at Runtime - Check DropDown binding and content
"""
import unreal

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("DIAGNOSING SETTINGS RUNTIME STATE")
    unreal.log_warning("=" * 70)

    # Load W_Settings WidgetBlueprint
    settings_bp = unreal.load_asset("/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings")
    if not settings_bp:
        unreal.log_error("Failed to load W_Settings!")
        return

    # Get the widget tree
    if not hasattr(settings_bp, 'widget_tree'):
        unreal.log_warning("W_Settings has no widget_tree attribute - checking generated class")

        gen_class = settings_bp.generated_class() if hasattr(settings_bp, 'generated_class') else None
        if gen_class:
            unreal.log_warning(f"Generated class: {gen_class.get_name()}")

            # Check CDO
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                unreal.log_warning(f"CDO class: {cdo.get_class().get_name()}")

                # Check for Settings entries
                for attr_name in dir(cdo):
                    if 'entry' in attr_name.lower() or 'setting' in attr_name.lower():
                        unreal.log_warning(f"  Attribute: {attr_name}")

    # Try loading W_Settings_Entry specifically
    unreal.log_warning("\n--- W_Settings_Entry Analysis ---")
    entry_bp = unreal.load_asset("/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_Entry")
    if not entry_bp:
        unreal.log_error("Failed to load W_Settings_Entry!")
        return

    # Check if it's reparented to C++
    gen_class = entry_bp.generated_class() if hasattr(entry_bp, 'generated_class') else None
    if gen_class:
        parent = unreal.SystemLibrary.get_super_class(gen_class)
        if parent:
            parent_name = parent.get_name()
            unreal.log_warning(f"W_Settings_Entry parent class: {parent_name}")

            # Check if parent is native C++
            if 'W_Settings_Entry' in parent_name and '_C' not in parent_name:
                unreal.log_warning("  -> Reparented to C++ UW_Settings_Entry correctly!")
            else:
                unreal.log_warning(f"  -> NOT reparented to C++! Parent is: {parent_name}")

        # Check CDO for widget bindings
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            unreal.log_warning(f"CDO: {cdo.get_name()}")

            # Check key properties
            props_to_check = [
                'setting_tag', 'setting_name', 'entry_type',
                'drop_down', 'dropdown_view', 'button_view', 'slider_view'
            ]
            for prop in props_to_check:
                try:
                    val = cdo.get_editor_property(prop)
                    unreal.log_warning(f"  {prop}: {val}")
                except:
                    unreal.log_warning(f"  {prop}: <not found>")

    unreal.log_warning("=" * 70)
    unreal.log_warning("DONE - Check logs for widget binding info")
    unreal.log_warning("=" * 70)

main()
