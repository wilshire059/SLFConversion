"""Test settings widget configuration."""
import unreal

def test_settings_widgets():
    unreal.log_warning("=== Testing Settings Widget Configuration ===")

    # Load W_Settings
    bp_path = '/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings'
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_warning(f"Failed to load {bp_path}")
        return

    gen_class = bp.generated_class()
    if not gen_class:
        unreal.log_warning("No generated class")
        return

    # Get widget tree
    try:
        widget_tree = bp.get_editor_property('widget_tree')
        if widget_tree:
            all_widgets = widget_tree.get_editor_property('all_widgets')
            if all_widgets:
                unreal.log_warning(f"Total widgets in tree: {len(all_widgets)}")

                # Find W_Settings_Entry instances
                entry_count = 0
                category_count = 0
                for widget in all_widgets:
                    if not widget:
                        continue
                    class_name = widget.get_class().get_name()
                    widget_name = widget.get_name()

                    if 'W_Settings_Entry' in class_name and 'CategoryEntry' not in class_name:
                        entry_count += 1
                        # Check if this is a C++ class instance
                        is_cpp = 'W_Settings_Entry' == widget.get_class().get_name()
                        unreal.log_warning(f"  Entry: {widget_name} (class: {class_name}, is_cpp: {is_cpp})")

                        # Try to get entry type
                        try:
                            entry_type = widget.get_editor_property('entry_type')
                            setting_name = widget.get_editor_property('setting_name')
                            unreal.log_warning(f"    EntryType: {entry_type}, SettingName: {setting_name}")
                        except Exception as e:
                            unreal.log_warning(f"    Could not get properties: {e}")

                    elif 'W_Settings_CategoryEntry' in class_name:
                        category_count += 1
                        unreal.log_warning(f"  Category: {widget_name} (class: {class_name})")

                        # Try to get switcher index
                        try:
                            switcher_index = widget.get_editor_property('switcher_index')
                            icon = widget.get_editor_property('icon')
                            unreal.log_warning(f"    SwitcherIndex: {switcher_index}, Icon: {icon}")
                        except Exception as e:
                            unreal.log_warning(f"    Could not get properties: {e}")

                unreal.log_warning(f"Found {entry_count} entries, {category_count} categories")
    except Exception as e:
        unreal.log_warning(f"Error accessing widget_tree: {e}")

    unreal.log_warning("=== Done ===")

test_settings_widgets()
