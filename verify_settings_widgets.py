"""
Verify W_Settings widget bindings after C++ rebuild.

Run with:
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/SLFConversion/SLFConversion.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/verify_settings_widgets.py" ^
  -stdout -unattended -nosplash 2>&1
"""

import unreal

def verify_w_settings_entry():
    """Verify W_Settings_Entry widget bindings work correctly."""
    unreal.log_warning("=" * 60)
    unreal.log_warning("W_Settings_Entry Widget Binding Verification")
    unreal.log_warning("=" * 60)

    bp_path = "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_Entry"
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_warning(f"FAIL: Could not load {bp_path}")
        return False

    gen_class = bp.generated_class()
    if not gen_class:
        unreal.log_warning("FAIL: No generated class")
        return False

    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        unreal.log_warning("FAIL: No CDO")
        return False

    unreal.log_warning(f"Loaded: {bp.get_name()}")
    unreal.log_warning("")

    # Check C++ properties that should exist after reparenting
    widget_bindings = [
        'background_border',       # Was: selected_border
        'setting_name_text',
        'slider_value',            # Was: value_text
        'buttons_value',           # NEW - for dropdown/button views
        'slider',                  # Was: value_slider
        'decrease_button',         # Was: left_arrow
        'increase_button',         # Was: right_arrow
        'button_view',             # NEW - view panels
        'double_button_view',
        'slider_view',
        'dropdown_view',
        'key_selector_view',
    ]

    success_count = 0
    for prop in widget_bindings:
        try:
            val = cdo.get_editor_property(prop)
            # Widget bindings are typically None in CDO but should be accessible
            status = "OK (accessible)"
            success_count += 1
        except Exception as e:
            status = f"ERROR - {e}"

        unreal.log_warning(f"  {prop}: {status}")

    unreal.log_warning("")
    unreal.log_warning(f"Result: {success_count}/{len(widget_bindings)} properties accessible")

    if success_count == len(widget_bindings):
        unreal.log_warning("SUCCESS: All widget bindings are accessible!")
        return True
    else:
        unreal.log_warning("PARTIAL: Some widget bindings not accessible")
        return False


def verify_w_settings():
    """Verify W_Settings widget bindings."""
    unreal.log_warning("")
    unreal.log_warning("=" * 60)
    unreal.log_warning("W_Settings Widget Binding Verification")
    unreal.log_warning("=" * 60)

    bp_path = "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings"
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_warning(f"FAIL: Could not load {bp_path}")
        return False

    gen_class = bp.generated_class()
    if not gen_class:
        unreal.log_warning("FAIL: No generated class")
        return False

    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        unreal.log_warning("FAIL: No CDO")
        return False

    props = [
        'categories_box',
        'category_switcher',
    ]

    success_count = 0
    for prop in props:
        try:
            val = cdo.get_editor_property(prop)
            status = "OK"
            success_count += 1
        except Exception as e:
            status = f"ERROR - {e}"

        unreal.log_warning(f"  {prop}: {status}")

    return success_count == len(props)


def verify_w_settings_category_entry():
    """Verify W_Settings_CategoryEntry widget bindings."""
    unreal.log_warning("")
    unreal.log_warning("=" * 60)
    unreal.log_warning("W_Settings_CategoryEntry Widget Binding Verification")
    unreal.log_warning("=" * 60)

    bp_path = "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_CategoryEntry"
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_warning(f"FAIL: Could not load {bp_path}")
        return False

    gen_class = bp.generated_class()
    if not gen_class:
        unreal.log_warning("FAIL: No generated class")
        return False

    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        unreal.log_warning("FAIL: No CDO")
        return False

    props = [
        'switcher_index',
        'selected',
        'setting_category',
        'selected_color',
        'deselected_color',
    ]

    success_count = 0
    for prop in props:
        try:
            val = cdo.get_editor_property(prop)
            status = f"OK ({val})"
            success_count += 1
        except Exception as e:
            status = f"ERROR - {e}"

        unreal.log_warning(f"  {prop}: {status}")

    return success_count == len(props)


def main():
    unreal.log_warning("=" * 60)
    unreal.log_warning("SETTINGS MENU VERIFICATION")
    unreal.log_warning("After C++ rebuild, all widget bindings should work")
    unreal.log_warning("=" * 60)

    results = []

    results.append(("W_Settings", verify_w_settings()))
    results.append(("W_Settings_CategoryEntry", verify_w_settings_category_entry()))
    results.append(("W_Settings_Entry", verify_w_settings_entry()))

    unreal.log_warning("")
    unreal.log_warning("=" * 60)
    unreal.log_warning("SUMMARY")
    unreal.log_warning("=" * 60)

    all_passed = True
    for name, passed in results:
        status = "PASS" if passed else "FAIL"
        if not passed:
            all_passed = False
        unreal.log_warning(f"  {name}: {status}")

    unreal.log_warning("")
    if all_passed:
        unreal.log_warning("ALL CHECKS PASSED - Settings menu should work correctly!")
        unreal.log_warning("")
        unreal.log_warning("Expected behavior after fix:")
        unreal.log_warning("  - Tab clicking: WORKS (CategoryEntry reparented)")
        unreal.log_warning("  - Value changing: WORKS (DecreaseButton/IncreaseButton bound)")
        unreal.log_warning("  - View switching: WORKS (visibility on view panels)")
        unreal.log_warning("  - Selection highlight: WORKS (BackgroundBorder color)")
    else:
        unreal.log_warning("SOME CHECKS FAILED - Review errors above")

    unreal.log_warning("=" * 60)


if __name__ == "__main__":
    main()
