"""
Clear W_Settings_Entry function graphs while preserving widget tree.

This removes the Blueprint function implementations (which conflict with C++)
but keeps the widget tree (DoubleButtonView, SliderView, etc.) intact.

Run with:
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/SLFConversion/SLFConversion.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/clear_settings_entry_graphs.py" ^
  -stdout -unattended -nosplash 2>&1
"""

import unreal


def clear_settings_entry_graphs():
    """Clear function graphs while preserving widget tree."""
    bp_path = "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_Entry"

    unreal.log_warning("=" * 60)
    unreal.log_warning("Clearing W_Settings_Entry function graphs (PRESERVING widget tree)")
    unreal.log_warning("=" * 60)

    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_warning(f"FAIL: Could not load {bp_path}")
        return False

    # Use SLFAutomationLibrary to clear event graph only
    if hasattr(unreal, 'SLFAutomationLibrary'):
        unreal.log_warning("Using SLFAutomationLibrary to clear event graph...")

        # ClearEventGraph removes function implementations but keeps widget tree
        result = unreal.SLFAutomationLibrary.clear_event_graph(bp)
        unreal.log_warning(f"ClearEventGraph result: {result}")

        if result:
            # Compile and save
            unreal.log_warning("Compiling and saving...")
            unreal.BlueprintEditorLibrary.compile_blueprint(bp)
            unreal.EditorAssetLibrary.save_asset(bp_path)
            unreal.log_warning("SUCCESS: W_Settings_Entry graphs cleared!")
            return True
        else:
            unreal.log_warning("FAIL: ClearEventGraph returned false")
            return False
    else:
        unreal.log_warning("FAIL: SLFAutomationLibrary not available")
        return False


def verify_widget_tree():
    """Verify widget tree is still intact."""
    unreal.log_warning("")
    unreal.log_warning("=== Verifying Widget Tree ===")

    bp_path = "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_Entry"
    bp = unreal.load_asset(bp_path)
    if not bp:
        return

    gen_class = bp.generated_class()
    if not gen_class:
        return

    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        return

    # Check that widget bindings still exist
    widget_bindings = [
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
        'drop_down',
    ]

    for widget in widget_bindings:
        try:
            val = cdo.get_editor_property(widget)
            status = "OK" if val is not None else "None (may be OK - binds at runtime)"
            unreal.log_warning(f"  {widget}: {status}")
        except Exception as e:
            unreal.log_warning(f"  {widget}: ERROR - {e}")


def check_compile():
    """Check compile status."""
    unreal.log_warning("")
    unreal.log_warning("=== Compile Check ===")

    bp = unreal.load_asset("/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_Entry")
    if bp:
        try:
            unreal.BlueprintEditorLibrary.compile_blueprint(bp)
            unreal.log_warning("W_Settings_Entry: Compiles OK")
        except Exception as e:
            unreal.log_warning(f"W_Settings_Entry: COMPILE ERROR - {e}")


if __name__ == "__main__":
    if clear_settings_entry_graphs():
        verify_widget_tree()
        check_compile()
    unreal.log_warning("=" * 60)
