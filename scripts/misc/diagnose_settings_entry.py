"""
Diagnose W_Settings_Entry widget issues.

Run with:
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/SLFConversion/SLFConversion.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/diagnose_settings_entry.py" ^
  -stdout -unattended -nosplash 2>&1
"""

import unreal

def diagnose_w_settings_entry():
    """Check W_Settings_Entry widget bindings and EntryType."""
    unreal.log_warning("=" * 70)
    unreal.log_warning("W_Settings_Entry Diagnostic")
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

    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        unreal.log_warning("FAIL: No CDO")
        return

    unreal.log_warning(f"Blueprint: {bp.get_name()}")
    unreal.log_warning(f"Generated Class: {gen_class.get_name()}")
    unreal.log_warning("")

    # Check parent class
    parent = gen_class.get_super_class()
    unreal.log_warning(f"Parent Class: {parent.get_name() if parent else 'None'}")
    unreal.log_warning("")

    # Check key C++ properties
    unreal.log_warning("=== C++ Properties (should be accessible if reparented) ===")
    cpp_props = [
        ('entry_type', 'EntryType'),
        ('setting_name', 'SettingName'),
        ('current_value', 'CurrentValue'),
        ('unhovered_color', 'UnhoveredColor'),
        ('hovered_color', 'HoveredColor'),
    ]

    for py_name, display_name in cpp_props:
        try:
            val = cdo.get_editor_property(py_name)
            unreal.log_warning(f"  {display_name}: {val}")
        except Exception as e:
            unreal.log_warning(f"  {display_name}: ERROR - {e}")

    unreal.log_warning("")
    unreal.log_warning("=== Widget Bindings (BindWidgetOptional) ===")
    widget_props = [
        ('background_border', 'BackgroundBorder'),
        ('setting_name_text', 'SettingNameText'),
        ('slider_value', 'SliderValue'),
        ('buttons_value', 'ButtonsValue'),
        ('slider', 'Slider'),
        ('decrease_button', 'DecreaseButton'),
        ('increase_button', 'IncreaseButton'),
        ('button_view', 'ButtonView'),
        ('double_button_view', 'DoubleButtonView'),
        ('slider_view', 'SliderView'),
        ('dropdown_view', 'DropdownView'),
        ('key_selector_view', 'KeySelectorView'),
    ]

    for py_name, display_name in widget_props:
        try:
            val = cdo.get_editor_property(py_name)
            status = f"OK (CDO value: {val})"
        except Exception as e:
            status = f"ERROR - {e}"
        unreal.log_warning(f"  {display_name}: {status}")

    unreal.log_warning("")
    unreal.log_warning("=" * 70)


def check_w_settings_instances():
    """Check W_Settings to see how entry instances are configured."""
    unreal.log_warning("")
    unreal.log_warning("=" * 70)
    unreal.log_warning("W_Settings Entry Instances")
    unreal.log_warning("=" * 70)

    bp_path = "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings"
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_warning(f"FAIL: Could not load {bp_path}")
        return

    # Try to get widget tree info via export
    # This is tricky in Python - let's just check what we can

    unreal.log_warning("Checking W_Settings Blueprint...")
    unreal.log_warning("(Widget instance properties must be checked in editor or via T3D export)")
    unreal.log_warning("")

    # Check if the entry type enum exists
    unreal.log_warning("=== ESLFSettingEntry Enum Values ===")
    try:
        # Try to access enum
        enum_class = unreal.find_object(None, "/Script/SLFConversion.ESLFSettingEntry")
        if enum_class:
            unreal.log_warning(f"  Enum found: {enum_class}")
        else:
            unreal.log_warning("  Enum not found via find_object")
    except Exception as e:
        unreal.log_warning(f"  Error: {e}")

    unreal.log_warning("")
    unreal.log_warning("Expected EntryType per setting:")
    unreal.log_warning("  ScreenMode: DropDown (0)")
    unreal.log_warning("  Resolution: DropDown (0)")
    unreal.log_warning("  TextureQuality: DoubleButton (2)")
    unreal.log_warning("  AntialiasingQuality: DoubleButton (2)")
    unreal.log_warning("  PostprocessQuality: DoubleButton (2)")
    unreal.log_warning("  ShadowQuality: DoubleButton (2)")
    unreal.log_warning("  ReflectionQuality: DoubleButton (2)")
    unreal.log_warning("  ShaderQuality: DoubleButton (2)")
    unreal.log_warning("  GlobalIlluminationQuality: DoubleButton (2)")
    unreal.log_warning("  ViewDistanceQuality: DoubleButton (2)")
    unreal.log_warning("  FoliageQuality: DoubleButton (2)")
    unreal.log_warning("")
    unreal.log_warning("=" * 70)


def main():
    diagnose_w_settings_entry()
    check_w_settings_instances()

    unreal.log_warning("")
    unreal.log_warning("=" * 70)
    unreal.log_warning("DIAGNOSIS COMPLETE")
    unreal.log_warning("=" * 70)
    unreal.log_warning("")
    unreal.log_warning("If widget bindings show ERROR, the Blueprint is NOT reparented to C++.")
    unreal.log_warning("If bindings show OK but views still hidden, check:")
    unreal.log_warning("  1. EntryType value on each instance")
    unreal.log_warning("  2. Initial visibility in Blueprint designer")
    unreal.log_warning("  3. SetEntryType being called in NativeConstruct")
    unreal.log_warning("")


if __name__ == "__main__":
    main()
