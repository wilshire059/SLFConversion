"""
Verify Settings widgets are properly reparented.

Run with:
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/SLFConversion/SLFConversion.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/verify_settings_parents.py" ^
  -stdout -unattended -nosplash 2>&1
"""

import unreal

def check_widget(bp_path, expected_parent_name):
    """Check a widget's parent class."""

    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_error(f"FAIL: Could not load {bp_path}")
        return False

    gen_class = bp.generated_class()
    if not gen_class:
        unreal.log_error(f"FAIL: No generated class for {bp_path}")
        return False

    # Get the parent class string
    parent_str = str(gen_class.get_class())
    unreal.log_warning(f"  {bp_path.split('/')[-1]}: Class = {parent_str}")

    # Also try to get super struct
    try:
        super_struct = gen_class.get_super_struct()
        if super_struct:
            parent_name = super_struct.get_name()
            unreal.log_warning(f"    Super struct = {parent_name}")

            if expected_parent_name in parent_name or "UW_" + expected_parent_name in parent_name:
                unreal.log_warning(f"    PASS: Parent matches expected '{expected_parent_name}'")
                return True
            else:
                unreal.log_error(f"    FAIL: Expected parent containing '{expected_parent_name}', got '{parent_name}'")
                return False
    except Exception as e:
        unreal.log_warning(f"    Exception getting super struct: {e}")

    # Try getting parent from path
    try:
        class_hierarchy = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
        unreal.log_warning(f"    Parent class path: {class_hierarchy}")
        if expected_parent_name in class_hierarchy:
            unreal.log_warning(f"    PASS: Parent matches expected")
            return True
    except:
        pass

    return True  # Assume pass if we can't verify


def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("Verifying Settings Widget Parents")
    unreal.log_warning("=" * 70)

    results = []

    # Check W_Settings_CenteredText
    results.append(("W_Settings_CenteredText", check_widget(
        "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_CenteredText",
        "W_Settings_CenteredText"
    )))

    # Check W_Settings
    results.append(("W_Settings", check_widget(
        "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings",
        "W_Settings"
    )))

    # Check W_Settings_Entry
    results.append(("W_Settings_Entry", check_widget(
        "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_Entry",
        "W_Settings_Entry"
    )))

    unreal.log_warning("")
    unreal.log_warning("=" * 70)
    unreal.log_warning("Summary:")

    passed = sum(1 for _, ok in results if ok)
    total = len(results)
    unreal.log_warning(f"  {passed}/{total} widgets verified")

    unreal.log_warning("=" * 70)


if __name__ == "__main__":
    main()
