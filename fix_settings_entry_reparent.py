"""
Clear event graphs and reparent W_Settings_Entry Blueprint to C++ class.

Run with:
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/SLFConversion/SLFConversion.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/fix_settings_entry_reparent.py" ^
  -stdout -unattended -nosplash 2>&1
"""

import unreal


def clear_and_reparent(bp_path, new_parent_path):
    """Clear event graphs and reparent a widget Blueprint."""

    unreal.log_warning(f"Processing: {bp_path}")

    # Load the Blueprint
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_error(f"Failed to load {bp_path}")
        return False

    # Clear event graphs using SLFAutomationLibrary
    try:
        if hasattr(unreal, 'SLFAutomationLibrary'):
            unreal.log_warning("  Clearing graphs (keeping variables)...")
            result = unreal.SLFAutomationLibrary.clear_graphs_keep_variables(bp)
            unreal.log_warning(f"  Clear result: {result}")
        else:
            unreal.log_warning("  SLFAutomationLibrary not found, trying BlueprintEditorLibrary...")
    except Exception as e:
        unreal.log_warning(f"  Clear graphs exception: {e}")

    # Load the new parent class
    new_parent = unreal.load_class(None, new_parent_path)
    if not new_parent:
        unreal.log_error(f"Failed to load parent class {new_parent_path}")
        return False

    unreal.log_warning(f"  New parent: {new_parent.get_name()}")

    # Reparent
    try:
        result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, new_parent)
        unreal.log_warning(f"  Reparent result: {result}")

        # Compile
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        unreal.log_warning(f"  Compiled")

        # Save
        unreal.EditorAssetLibrary.save_asset(bp_path)
        unreal.log_warning(f"  Saved")
        return True
    except Exception as e:
        unreal.log_error(f"  Exception: {e}")
        return False


def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("Fixing W_Settings_Entry - Clear and Reparent")
    unreal.log_warning("=" * 70)

    success = clear_and_reparent(
        "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_Entry",
        "/Script/SLFConversion.W_Settings_Entry"
    )

    if success:
        unreal.log_warning("SUCCESS!")
    else:
        unreal.log_error("FAILED!")

    unreal.log_warning("=" * 70)


if __name__ == "__main__":
    main()
