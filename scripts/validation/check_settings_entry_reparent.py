"""
Check if W_Settings_Entry is properly reparented to C++.

If not reparented, the increase/decrease functionality won't work.

Run with:
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/SLFConversion/SLFConversion.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/check_settings_entry_reparent.py" ^
  -stdout -unattended -nosplash 2>&1
"""

import unreal

def check_blueprint_reparenting(bp_path, cpp_class_path):
    """Check if Blueprint is reparented to C++ by testing property access."""
    unreal.log_warning(f"Checking: {bp_path}")

    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_warning(f"  FAIL: Could not load Blueprint")
        return False, "not_loaded"

    gen_class = bp.generated_class()
    if not gen_class:
        unreal.log_warning(f"  FAIL: No generated class")
        return False, "no_gen_class"

    unreal.log_warning(f"  Generated class: {gen_class.get_name()}")

    # Try to load the C++ class
    cpp_class = unreal.load_class(None, cpp_class_path)
    if not cpp_class:
        unreal.log_warning(f"  WARN: C++ class not found: {cpp_class_path}")
        return False, "no_cpp_class"

    unreal.log_warning(f"  C++ class: {cpp_class.get_name()}")

    # Check CDO for C++ properties
    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        unreal.log_warning(f"  FAIL: No CDO")
        return False, "no_cdo"

    # Test specific properties that should exist if reparented
    test_props = {
        'W_Settings_Entry': ['setting_tag', 'setting_name', 'entry_type', 'current_value'],
        'W_Settings_CategoryEntry': ['switcher_index', 'selected', 'selected_color'],
        'W_Settings': ['categories_box', 'category_switcher'],
    }

    class_name = cpp_class_path.split('.')[-1]
    props_to_test = test_props.get(class_name, [])

    found_props = 0
    for prop in props_to_test:
        try:
            val = cdo.get_editor_property(prop)
            unreal.log_warning(f"    {prop}: {val} (OK)")
            found_props += 1
        except Exception as e:
            unreal.log_warning(f"    {prop}: ERROR - {e}")

    if found_props > 0:
        unreal.log_warning(f"  PASS: {found_props}/{len(props_to_test)} C++ properties accessible")
        return True, "reparented"
    else:
        unreal.log_warning(f"  FAIL: No C++ properties accessible - NOT REPARENTED")
        return False, "not_reparented"


def main():
    unreal.log_warning("=" * 60)
    unreal.log_warning("Settings Widget Reparenting Check")
    unreal.log_warning("=" * 60)

    widgets = [
        ("/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings", "/Script/SLFConversion.W_Settings"),
        ("/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_CategoryEntry", "/Script/SLFConversion.W_Settings_CategoryEntry"),
        ("/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_Entry", "/Script/SLFConversion.W_Settings_Entry"),
    ]

    results = []
    for bp_path, cpp_path in widgets:
        unreal.log_warning("")
        ok, status = check_blueprint_reparenting(bp_path, cpp_path)
        results.append((bp_path.split('/')[-1], ok, status))

    unreal.log_warning("")
    unreal.log_warning("=" * 60)
    unreal.log_warning("SUMMARY")
    unreal.log_warning("=" * 60)

    for name, ok, status in results:
        status_str = "OK" if ok else f"FAIL ({status})"
        unreal.log_warning(f"  {name}: {status_str}")

    unreal.log_warning("")

    # Check if W_Settings_Entry needs reparenting
    entry_result = next((r for r in results if "Entry" in r[0]), None)
    if entry_result and not entry_result[1]:
        unreal.log_warning("W_Settings_Entry is NOT reparented!")
        unreal.log_warning("This is why values cannot be changed.")
        unreal.log_warning("The increase/decrease buttons won't trigger C++ handlers.")

    unreal.log_warning("=" * 60)


if __name__ == "__main__":
    main()
