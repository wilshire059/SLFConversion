"""
Diagnose Embedded Settings Widgets - Check actual embedded widget property values
"""
import unreal

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("DIAGNOSING EMBEDDED W_SETTINGS WIDGETS")
    unreal.log_warning("=" * 70)

    # Load W_Settings WidgetBlueprint
    settings_bp = unreal.load_asset("/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings")
    if not settings_bp:
        unreal.log_error("Failed to load W_Settings!")
        return

    unreal.log_warning(f"Loaded: {settings_bp.get_name()}")
    unreal.log_warning(f"Class: {settings_bp.get_class().get_name()}")

    # Get WidgetTree
    widget_tree = settings_bp.widget_tree
    if not widget_tree:
        unreal.log_error("No WidgetTree!")
        return

    # Get all widgets
    all_widgets = widget_tree.get_all_widgets()
    unreal.log_warning(f"Total widgets in tree: {len(all_widgets)}")

    # Check each W_Settings_Entry embedded widget
    entry_count = 0
    for widget in all_widgets:
        class_name = widget.get_class().get_name()
        if "W_Settings_Entry" in class_name and "CategoryEntry" not in class_name:
            entry_count += 1
            widget_name = widget.get_name()

            # Try to read properties
            try:
                setting_tag = widget.get_editor_property("setting_tag")
                setting_name = widget.get_editor_property("setting_name")
                entry_type = widget.get_editor_property("entry_type")

                # EntryType enum to string
                entry_type_str = str(entry_type) if entry_type else "None"

                unreal.log_warning(f"\n--- {widget_name} ---")
                unreal.log_warning(f"  Class: {class_name}")
                unreal.log_warning(f"  SettingTag: {setting_tag}")
                unreal.log_warning(f"  SettingName: {setting_name}")
                unreal.log_warning(f"  EntryType: {entry_type_str}")

                # Check if it has the DropDown widget
                # This is tricky - we can't easily access child widgets from here
            except Exception as e:
                unreal.log_warning(f"\n--- {widget_name} ---")
                unreal.log_warning(f"  ERROR reading properties: {e}")

    unreal.log_warning(f"\nTotal W_Settings_Entry widgets found: {entry_count}")
    unreal.log_warning("=" * 70)

main()
