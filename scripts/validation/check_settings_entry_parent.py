"""
Check W_Settings_Entry parent class.

Run with:
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/SLFConversion/SLFConversion.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/check_settings_entry_parent.py" ^
  -stdout -unattended -nosplash 2>&1
"""

import unreal


def check_parent(bp_path):
    """Check a Blueprint's parent class."""

    unreal.log_warning(f"Loading {bp_path}")

    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_error(f"Failed to load {bp_path}")
        return

    unreal.log_warning(f"Asset type: {type(bp)}")

    gen_class = bp.generated_class()
    if gen_class:
        unreal.log_warning(f"Generated class: {gen_class.get_name()}")
        parent = gen_class.get_super_struct()
        if parent:
            unreal.log_warning(f"Parent class: {parent.get_name()}")
            parent_path = parent.get_path_name()
            unreal.log_warning(f"Parent path: {parent_path}")

            # Check if it's C++ or Blueprint
            if "/Script/" in parent_path:
                unreal.log_warning("Parent is C++ class!")
            else:
                unreal.log_warning("Parent is Blueprint class")
    else:
        unreal.log_error("No generated class - Blueprint likely has errors")


def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("Checking W_Settings_Entry Parent Class")
    unreal.log_warning("=" * 70)

    check_parent("/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_Entry")

    unreal.log_warning("=" * 70)
    unreal.log_warning("Done!")
    unreal.log_warning("=" * 70)


if __name__ == "__main__":
    main()
