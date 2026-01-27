"""
Reparent W_Settings_Entry Blueprint to C++ UW_Settings_Entry class.

This fixes the issue where the Blueprint was restored from bp_only with parent UUserWidget,
but the C++ class UW_Settings_Entry exists and needs to be the parent for proper integration.

Run with:
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/SLFConversion/SLFConversion.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/reparent_settings_entry.py" ^
  -stdout -unattended -nosplash 2>&1
"""

import unreal


def reparent_widget(bp_path, new_parent_path):
    """Reparent a widget Blueprint to a new C++ parent class."""

    unreal.log_warning(f"Reparenting: {bp_path}")
    unreal.log_warning(f"  New parent: {new_parent_path}")

    # Load the Blueprint
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_error(f"Failed to load {bp_path}")
        return False

    # Load the new parent class
    new_parent = unreal.load_class(None, new_parent_path)
    if not new_parent:
        unreal.log_error(f"Failed to load parent class {new_parent_path}")
        return False

    unreal.log_warning(f"  Parent class loaded: {new_parent.get_name()}")

    # Reparent using BlueprintEditorLibrary
    try:
        result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, new_parent)
        unreal.log_warning(f"  Reparent result: {result}")

        if result:
            # Compile the Blueprint
            unreal.BlueprintEditorLibrary.compile_blueprint(bp)
            unreal.log_warning(f"  Compiled Blueprint")

            # Save the asset
            unreal.EditorAssetLibrary.save_asset(bp_path)
            unreal.log_warning(f"  Saved {bp_path}")
            return True
        else:
            unreal.log_error(f"  Reparent failed!")
            return False
    except Exception as e:
        unreal.log_error(f"  Exception during reparent: {e}")
        return False


def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("Reparenting Settings Widgets to C++ Classes")
    unreal.log_warning("=" * 70)

    # Reparent W_Settings_Entry to UW_Settings_Entry
    success = reparent_widget(
        "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_Entry",
        "/Script/SLFConversion.W_Settings_Entry"
    )

    if success:
        unreal.log_warning("SUCCESS: W_Settings_Entry reparented to C++")
    else:
        unreal.log_error("FAILED: W_Settings_Entry reparent")

    unreal.log_warning("=" * 70)
    unreal.log_warning("Done!")
    unreal.log_warning("=" * 70)


if __name__ == "__main__":
    main()
