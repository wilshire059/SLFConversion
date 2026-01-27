"""
Diagnose W_Settings Display tab - export actual widget state.

Run with:
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/SLFConversion/SLFConversion.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/diagnose_settings_display.py" ^
  -stdout -unattended -nosplash 2>&1
"""

import unreal

def diagnose_w_settings_entry():
    """Export W_Settings_Entry widget state."""
    unreal.log_warning("=" * 70)
    unreal.log_warning("W_Settings_Entry DIAGNOSTIC")
    unreal.log_warning("=" * 70)

    bp_path = "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_Entry"
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_warning(f"FAIL: Could not load {bp_path}")
        return

    gen_class = bp.generated_class()
    if not gen_class:
        unreal.log_warning("FAIL: No generated class")
        return

    # Get parent class
    parent = gen_class.get_super_class()
    unreal.log_warning(f"Parent Class: {parent.get_name() if parent else 'None'}")

    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        unreal.log_warning("FAIL: No CDO")
        return

    unreal.log_warning("")
    unreal.log_warning("=== CDO Properties ===")

    # Check EntryType
    try:
        entry_type = cdo.get_editor_property('entry_type')
        unreal.log_warning(f"  EntryType: {entry_type}")
    except Exception as e:
        unreal.log_warning(f"  EntryType: ERROR - {e}")

    # Check widget bindings
    widget_props = [
        'background_border',
        'setting_name_text',
        'slider_value',
        'buttons_value',
        'slider',
        'decrease_button',
        'increase_button',
        'button_view',
        'double_button_view',
        'slider_view',
        'dropdown_view',
        'key_selector_view',
        'drop_down',  # The ComboBox
    ]

    unreal.log_warning("")
    unreal.log_warning("=== Widget Bindings (CDO) ===")
    for prop in widget_props:
        try:
            val = cdo.get_editor_property(prop)
            unreal.log_warning(f"  {prop}: {val}")
        except Exception as e:
            unreal.log_warning(f"  {prop}: ERROR - {e}")


def diagnose_w_settings():
    """Export W_Settings widget tree to see entry configurations."""
    unreal.log_warning("")
    unreal.log_warning("=" * 70)
    unreal.log_warning("W_Settings DIAGNOSTIC - Entry Instances")
    unreal.log_warning("=" * 70)

    bp_path = "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings"
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_warning(f"FAIL: Could not load {bp_path}")
        return

    # Try to get widget tree via export
    unreal.log_warning("Exporting W_Settings to check entry instances...")

    # Export to text to see widget tree
    export_path = "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings"

    # Use asset tools to export
    try:
        text = unreal.EditorAssetLibrary.export_text(export_path)
        if text:
            # Look for W_Settings_Entry instances and their EntryType
            lines = text.split('\n')
            in_entry = False
            entry_name = ""
            for i, line in enumerate(lines):
                if 'W_Settings_Entry' in line and 'Class=' not in line:
                    in_entry = True
                    entry_name = line.strip()
                    unreal.log_warning(f"\nFound Entry: {entry_name[:80]}")
                elif in_entry and 'EntryType' in line:
                    unreal.log_warning(f"  {line.strip()}")
                elif in_entry and 'SettingName' in line:
                    unreal.log_warning(f"  {line.strip()}")
                elif in_entry and ('End Object' in line or line.strip() == ''):
                    in_entry = False
    except Exception as e:
        unreal.log_warning(f"Export failed: {e}")


def check_blueprint_compile_status():
    """Check if W_Settings_Entry compiles."""
    unreal.log_warning("")
    unreal.log_warning("=" * 70)
    unreal.log_warning("BLUEPRINT COMPILE STATUS")
    unreal.log_warning("=" * 70)

    bp_path = "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_Entry"
    bp = unreal.load_asset(bp_path)
    if not bp:
        return

    # Try to compile
    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        unreal.log_warning("W_Settings_Entry: Compiled successfully")
    except Exception as e:
        unreal.log_warning(f"W_Settings_Entry: COMPILE ERROR - {e}")

    # Check W_Settings too
    bp_path2 = "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings"
    bp2 = unreal.load_asset(bp_path2)
    if bp2:
        try:
            unreal.BlueprintEditorLibrary.compile_blueprint(bp2)
            unreal.log_warning("W_Settings: Compiled successfully")
        except Exception as e:
            unreal.log_warning(f"W_Settings: COMPILE ERROR - {e}")


def export_widget_tree_structure():
    """Export the actual widget tree from W_Settings_Entry."""
    unreal.log_warning("")
    unreal.log_warning("=" * 70)
    unreal.log_warning("W_Settings_Entry WIDGET TREE")
    unreal.log_warning("=" * 70)

    bp_path = "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_Entry"

    try:
        text = unreal.EditorAssetLibrary.export_text(bp_path)
        if text:
            lines = text.split('\n')
            # Find widget tree section
            for i, line in enumerate(lines):
                # Look for widget names that match our expected structure
                if any(name in line for name in ['DropDown', 'DecreaseButton', 'IncreaseButton',
                                                   'ButtonsValue', 'SliderValue', 'DoubleButtonView',
                                                   'DropdownView', 'SliderView', 'BackgroundBorder']):
                    unreal.log_warning(line.strip()[:100])
    except Exception as e:
        unreal.log_warning(f"Export failed: {e}")


def main():
    check_blueprint_compile_status()
    diagnose_w_settings_entry()
    export_widget_tree_structure()
    diagnose_w_settings()

    unreal.log_warning("")
    unreal.log_warning("=" * 70)
    unreal.log_warning("DIAGNOSTIC COMPLETE")
    unreal.log_warning("=" * 70)


if __name__ == "__main__":
    main()
