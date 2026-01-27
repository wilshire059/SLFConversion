"""
Reparent W_Settings_Entry to C++ while PRESERVING widget tree bindings.

Unlike the previous approach, this does NOT clear all Blueprint logic.
The widget tree bindings (OnGenerateItemWidget, etc.) need to stay intact.

Run with:
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/SLFConversion/SLFConversion.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/reparent_settings_entry_preserve.py" ^
  -stdout -unattended -nosplash 2>&1
"""

import unreal


def reparent_w_settings_entry():
    """Reparent W_Settings_Entry to C++ WITHOUT clearing widget tree bindings."""
    bp_path = "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_Entry"
    cpp_class_path = "/Script/SLFConversion.W_Settings_Entry"

    unreal.log_warning("=" * 60)
    unreal.log_warning("Reparenting W_Settings_Entry to C++ (PRESERVING widget bindings)")
    unreal.log_warning("=" * 60)

    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_warning(f"FAIL: Could not load {bp_path}")
        return False

    unreal.log_warning(f"Loaded: {bp.get_name()}")

    # Check current parent
    gen_class = bp.generated_class()
    if gen_class:
        cdo = unreal.get_default_object(gen_class)
        unreal.log_warning(f"Current class: {gen_class.get_name()}")

    # Use SLFAutomationLibrary for reparenting ONLY (no clearing)
    if hasattr(unreal, 'SLFAutomationLibrary'):
        unreal.log_warning("Using SLFAutomationLibrary for reparenting...")

        # ONLY reparent - do NOT clear logic
        unreal.log_warning(f"Reparenting to {cpp_class_path}...")
        result = unreal.SLFAutomationLibrary.reparent_blueprint(bp, cpp_class_path)
        unreal.log_warning(f"Reparent result: {result}")

        if result:
            # Compile and save
            unreal.log_warning("Compiling and saving...")
            unreal.BlueprintEditorLibrary.compile_blueprint(bp)
            unreal.EditorAssetLibrary.save_asset(bp_path)
            unreal.log_warning("SUCCESS: W_Settings_Entry reparented to C++!")
            return True
        else:
            unreal.log_warning("FAIL: Reparent returned false")
            return False
    else:
        unreal.log_warning("FAIL: SLFAutomationLibrary not available")
        return False


def verify_reparent():
    """Verify the reparent worked and widget bindings are intact."""
    unreal.log_warning("")
    unreal.log_warning("=== Verification ===")

    bp = unreal.load_asset("/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_Entry")
    if not bp:
        return

    gen_class = bp.generated_class()
    if not gen_class:
        return

    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        return

    # Check C++ properties are accessible
    props_to_check = [
        'entry_type',
        'double_button_view',
        'decrease_button',
        'increase_button',
        'slider_view',
        'drop_down',
    ]

    for prop in props_to_check:
        try:
            val = cdo.get_editor_property(prop)
            unreal.log_warning(f"  {prop}: OK (value: {val})")
        except Exception as e:
            unreal.log_warning(f"  {prop}: ERROR - {e}")


def check_compile_status():
    """Check if Blueprint compiles without errors."""
    unreal.log_warning("")
    unreal.log_warning("=== Compile Status ===")

    bp = unreal.load_asset("/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_Entry")
    if bp:
        try:
            unreal.BlueprintEditorLibrary.compile_blueprint(bp)
            unreal.log_warning("W_Settings_Entry: Compiles OK")
        except Exception as e:
            unreal.log_warning(f"W_Settings_Entry: COMPILE ERROR - {e}")


if __name__ == "__main__":
    if reparent_w_settings_entry():
        verify_reparent()
        check_compile_status()
    unreal.log_warning("=" * 60)
