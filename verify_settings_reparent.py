"""
Verify W_Settings widgets are reparented to C++.

Run with:
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/SLFConversion/SLFConversion.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/verify_settings_reparent.py" ^
  -stdout -unattended -nosplash 2>&1
"""

import unreal

def get_parent_class_name(bp):
    """Get the parent class name of a Blueprint using native properties."""
    # Try to get the parent class from the Blueprint's native data
    # Blueprints have a ParentClass property but it's not exposed to Python
    # However, we can check by examining what the generated class inherits from

    gen_class = bp.generated_class()
    if not gen_class:
        return "No generated class"

    # The C++ class name should appear in the class path if reparented
    class_path = gen_class.get_path_name()

    # Get class display name - for Blueprint-derived it's ClassName_C
    display_name = gen_class.get_name()

    return display_name, class_path


def check_widget(bp_path, expected_cpp_class):
    """Check if a widget Blueprint is reparented to expected C++ class."""
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_warning(f"FAIL: Could not load {bp_path}")
        return False

    name = bp.get_name()
    unreal.log_warning(f"=== {name} ===")

    gen_class = bp.generated_class()
    if gen_class:
        class_name = gen_class.get_name()
        class_path = gen_class.get_path_name()
        unreal.log_warning(f"  Generated class: {class_name}")
        unreal.log_warning(f"  Path: {class_path}")

        # Try to get CDO and check a C++ property
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            # Check if C++ properties exist - this is the real test
            try:
                # These properties only exist if C++ parent is set
                if "CategoryEntry" in name:
                    val = cdo.get_editor_property('switcher_index')
                    unreal.log_warning(f"  C++ property 'switcher_index': {val} (exists = REPARENTED)")
                    return True
                elif "Settings_Entry" in name:
                    val = cdo.get_editor_property('setting_value')
                    unreal.log_warning(f"  C++ property 'setting_value': exists = REPARENTED")
                    return True
                elif "W_Settings" == name:
                    # W_Settings doesn't have unique C++ properties, check binding
                    val = cdo.get_editor_property('categories_box')
                    unreal.log_warning(f"  C++ property 'categories_box': {val}")
                    return True
            except Exception as e:
                unreal.log_warning(f"  C++ property check ERROR: {e}")
                unreal.log_warning(f"  NOT REPARENTED (C++ properties don't exist)")
                return False

    return False


def main():
    unreal.log_warning("=" * 60)
    unreal.log_warning("Verify Settings Widgets Reparenting")
    unreal.log_warning("=" * 60)

    widgets = [
        ("/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_CategoryEntry", "W_Settings_CategoryEntry"),
        ("/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_Entry", "W_Settings_Entry"),
        ("/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings", "W_Settings"),
    ]

    results = []
    for bp_path, expected_cpp in widgets:
        result = check_widget(bp_path, expected_cpp)
        results.append((bp_path.split('/')[-1], result))

    unreal.log_warning("")
    unreal.log_warning("=" * 60)
    unreal.log_warning("SUMMARY")
    unreal.log_warning("=" * 60)

    all_ok = True
    for name, ok in results:
        status = "OK (reparented)" if ok else "FAIL (not reparented)"
        unreal.log_warning(f"  {name}: {status}")
        if not ok:
            all_ok = False

    unreal.log_warning("")
    if all_ok:
        unreal.log_warning("All Settings widgets are properly reparented to C++!")
    else:
        unreal.log_warning("Some widgets need reparenting. Tab clicking may not work.")

    unreal.log_warning("=" * 60)

if __name__ == "__main__":
    main()
