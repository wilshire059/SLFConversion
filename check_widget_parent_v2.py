"""
Check widget parent class using load_blueprint_class.

Run with:
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/SLFConversion/SLFConversion.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/check_widget_parent_v2.py" ^
  -stdout -unattended -nosplash 2>&1
"""

import unreal


def check_parent(bp_path):
    """Check a Blueprint's parent class using EditorAssetLibrary."""

    unreal.log_warning(f"Checking: {bp_path}")

    # Load as Blueprint
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_error(f"Failed to load {bp_path}")
        return

    # Get parent class
    try:
        # For WidgetBlueprint, get parent class
        parent_class = bp.get_editor_property("ParentClass")
        if parent_class:
            parent_name = parent_class.get_name()
            parent_path = parent_class.get_path_name()
            unreal.log_warning(f"  ParentClass: {parent_name}")
            unreal.log_warning(f"  ParentPath: {parent_path}")

            # Check if it's C++ or Blueprint
            if "/Script/" in parent_path:
                unreal.log_warning(f"  -> Parent is C++ class")
            else:
                unreal.log_warning(f"  -> Parent is Blueprint class")
        else:
            unreal.log_warning(f"  ParentClass: None")
    except Exception as e:
        unreal.log_warning(f"  Error getting ParentClass: {e}")


def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("Checking Widget Parent Classes")
    unreal.log_warning("=" * 70)

    check_parent("/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_Entry")
    check_parent("/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings")
    check_parent("/Game/SoulslikeFramework/Widgets/_Generic/W_GenericButton")

    unreal.log_warning("=" * 70)
    unreal.log_warning("Done!")
    unreal.log_warning("=" * 70)


if __name__ == "__main__":
    main()
