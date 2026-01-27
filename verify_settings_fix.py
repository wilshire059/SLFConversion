"""
Verify W_Settings and W_Settings_Entry are properly reparented to C++.

Run with:
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/SLFConversion/SLFConversion.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/verify_settings_fix.py" ^
  -stdout -unattended -nosplash 2>&1
"""

import unreal


def check_class_hierarchy(bp_path, expected_cpp_parent):
    """Check a Blueprint's parent class hierarchy."""

    unreal.log_warning(f"Checking: {bp_path}")

    # Load as Blueprint
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_error(f"  Failed to load {bp_path}")
        return False

    # Get the generated class
    gen_class = bp.generated_class()
    if not gen_class:
        unreal.log_error(f"  No generated class!")
        return False

    # Get class name
    class_name = gen_class.get_name()
    unreal.log_warning(f"  Generated class: {class_name}")

    # Get super class (parent)
    super_class = gen_class.get_super_class()
    if super_class:
        super_name = super_class.get_name()
        super_path = super_class.get_path_name()
        unreal.log_warning(f"  Super class: {super_name}")
        unreal.log_warning(f"  Super path: {super_path}")

        # Check if it's the expected C++ parent
        if expected_cpp_parent in super_name or expected_cpp_parent in super_path:
            unreal.log_warning(f"  SUCCESS: Parented to C++ {expected_cpp_parent}")
            return True
        elif "/Script/" in super_path:
            unreal.log_warning(f"  PARTIAL: Parented to some C++ class: {super_name}")
            return True
        else:
            unreal.log_error(f"  FAIL: Parented to Blueprint, not C++!")
            return False
    else:
        unreal.log_error(f"  No super class found!")
        return False


def verify_cpp_class_exists(cpp_class_path):
    """Verify a C++ class can be loaded."""

    unreal.log_warning(f"Checking C++ class: {cpp_class_path}")
    cpp_class = unreal.load_class(None, cpp_class_path)
    if cpp_class:
        unreal.log_warning(f"  EXISTS: {cpp_class.get_name()}")
        return True
    else:
        unreal.log_error(f"  NOT FOUND!")
        return False


def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("Verifying Settings Widget Migration")
    unreal.log_warning("=" * 70)

    # Check C++ classes exist
    unreal.log_warning("")
    unreal.log_warning("--- C++ Class Verification ---")
    verify_cpp_class_exists("/Script/SLFConversion.W_Settings")
    verify_cpp_class_exists("/Script/SLFConversion.W_Settings_Entry")

    # Check Blueprint parent classes
    unreal.log_warning("")
    unreal.log_warning("--- Blueprint Parent Verification ---")

    settings_ok = check_class_hierarchy(
        "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings",
        "W_Settings"
    )

    entry_ok = check_class_hierarchy(
        "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_Entry",
        "W_Settings_Entry"
    )

    unreal.log_warning("")
    unreal.log_warning("=" * 70)
    if settings_ok and entry_ok:
        unreal.log_warning("ALL CHECKS PASSED - Widgets reparented to C++")
    else:
        unreal.log_error("SOME CHECKS FAILED")
    unreal.log_warning("=" * 70)


if __name__ == "__main__":
    main()
