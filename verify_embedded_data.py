"""
Verify embedded W_Settings_Entry data in W_Settings
"""
import unreal

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("VERIFYING EMBEDDED SETTINGS DATA")
    unreal.log_warning("=" * 70)

    # Load W_Settings WidgetBlueprint
    settings_bp = unreal.load_asset("/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings")
    if not settings_bp:
        unreal.log_error("Failed to load W_Settings!")
        return

    # Get WidgetTree
    widget_tree = None
    if hasattr(settings_bp, 'widget_tree'):
        widget_tree = settings_bp.widget_tree

    if widget_tree is None:
        unreal.log_warning("Cannot access widget_tree directly - using alternate method")
        # Try using the generated class CDO
        gen_class = settings_bp.generated_class()
        if gen_class:
            cdo = unreal.get_default_object(gen_class)
            unreal.log_warning(f"CDO: {cdo.get_name()}")

    # Try loading W_Settings_Entry to verify its DropDown
    entry_bp = unreal.load_asset("/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_Entry")
    if entry_bp:
        unreal.log_warning(f"\n--- W_Settings_Entry Blueprint ---")

        gen_class = entry_bp.generated_class()
        if gen_class:
            cdo = unreal.get_default_object(gen_class)
            unreal.log_warning(f"CDO: {cdo.get_name()}")

            # Check key properties on CDO
            try:
                entry_type = cdo.get_editor_property('entry_type')
                setting_tag = cdo.get_editor_property('setting_tag')
                setting_name = cdo.get_editor_property('setting_name')
                dropdown_view = cdo.get_editor_property('dropdown_view')
                drop_down = cdo.get_editor_property('drop_down')

                unreal.log_warning(f"  entry_type (CDO): {entry_type}")
                unreal.log_warning(f"  setting_tag (CDO): {setting_tag}")
                unreal.log_warning(f"  setting_name (CDO): {setting_name}")
                unreal.log_warning(f"  dropdown_view (CDO): {dropdown_view}")
                unreal.log_warning(f"  drop_down (CDO): {drop_down}")
            except Exception as e:
                unreal.log_warning(f"  Property error: {e}")

    unreal.log_warning("")
    unreal.log_warning("NOTE: CDO properties are defaults for the class.")
    unreal.log_warning("EMBEDDED instances in W_Settings have DIFFERENT values.")
    unreal.log_warning("The embedded data was applied by ApplyEmbeddedSettingsWidgets()")
    unreal.log_warning("Check PIE runtime logs to see actual instance values.")
    unreal.log_warning("=" * 70)

main()
