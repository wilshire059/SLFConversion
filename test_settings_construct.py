"""Test W_Settings construction by creating a widget instance."""
import unreal

def main():
    unreal.log_warning("=== Testing W_Settings Construction ===")

    # Load W_Settings Blueprint
    bp_path = '/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings'
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_warning(f"Failed to load {bp_path}")
        return

    # Get generated class
    gen_class = bp.generated_class()
    if not gen_class:
        unreal.log_warning("No generated class")
        return

    unreal.log_warning(f"Generated class: {gen_class.get_name()}")

    # Create widget instance (this triggers NativeConstruct)
    # Need a world context - use editor world
    editor_subsystem = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    world = editor_subsystem.get_editor_world()

    if not world:
        unreal.log_warning("No editor world")
        return

    # Create the widget
    unreal.log_warning("Creating widget instance...")
    widget_instance = unreal.WidgetBlueprintLibrary.create(gen_class, world.get_world_settings())

    if widget_instance:
        unreal.log_warning(f"Widget created successfully: {widget_instance.get_name()}")

        # Check if entries were configured
        # Try to access SettingEntries array
        try:
            setting_entries = widget_instance.get_editor_property('setting_entries')
            unreal.log_warning(f"SettingEntries count: {len(setting_entries) if setting_entries else 0}")
        except Exception as e:
            unreal.log_warning(f"Could not access setting_entries: {e}")

        try:
            category_entries = widget_instance.get_editor_property('category_entries')
            unreal.log_warning(f"CategoryEntries count: {len(category_entries) if category_entries else 0}")
        except Exception as e:
            unreal.log_warning(f"Could not access category_entries: {e}")

        # Clean up
        widget_instance.remove_from_parent()
    else:
        unreal.log_warning("Failed to create widget instance")

    unreal.log_warning("=== Done ===")

main()
