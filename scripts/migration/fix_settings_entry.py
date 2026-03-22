"""
Fix W_Settings_Entry reparenting and W_Settings parent class.

W_Settings_Entry is still using Blueprint parent - needs to be reparented to C++.
W_Settings is using UW_Navigable_InputReader but should use UW_Settings.

Run with:
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/SLFConversion/SLFConversion.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/fix_settings_entry.py" ^
  -stdout -unattended -nosplash 2>&1
"""

import unreal

def fix_widget(bp_path, cpp_path, clear_all=True):
    """Fix a single widget by clearing logic and reparenting."""
    name = bp_path.split('/')[-1]
    unreal.log_warning(f"")
    unreal.log_warning(f"=== Fixing {name} ===")

    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_warning(f"  ERROR: Could not load")
        return False

    # Get current parent
    gen_class = bp.generated_class()
    if gen_class:
        try:
            parent_name = str(gen_class.get_class()).split('.')[-1] if gen_class.get_class() else "Unknown"
            unreal.log_warning(f"  Current parent type: {type(gen_class)}")
        except:
            pass

    # Clear logic
    if clear_all:
        unreal.log_warning(f"  Clearing ALL Blueprint logic...")
        unreal.SLFAutomationLibrary.clear_all_blueprint_logic_no_compile(bp)
    else:
        unreal.log_warning(f"  Clearing EventGraphs only...")
        count = unreal.SLFAutomationLibrary.clear_event_graphs(bp)
        unreal.log_warning(f"    Cleared {count} event graphs")

    # Save after clear
    unreal.EditorAssetLibrary.save_asset(bp_path)

    # Reload
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_warning(f"  ERROR: Could not reload after clear")
        return False

    # Reparent
    unreal.log_warning(f"  Reparenting to {cpp_path.split('.')[-1]}...")
    ok = unreal.SLFAutomationLibrary.reparent_blueprint(bp, cpp_path)
    if not ok:
        unreal.log_warning(f"  ERROR: Reparent failed")
        return False

    # Compile and save
    unreal.log_warning(f"  Compiling...")
    result = unreal.SLFAutomationLibrary.compile_and_save(bp)
    if result:
        unreal.log_warning(f"  SUCCESS")
        return True
    else:
        # Get compile errors
        errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
        unreal.log_warning(f"  COMPILE ERRORS: {errors}")
        return False


def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("Fixing Settings Widgets")
    unreal.log_warning("=" * 70)

    # Fix W_Settings_CenteredText FIRST - W_Settings_Entry depends on this
    # This widget creates the centered text items for ComboBox dropdowns
    fix_widget(
        "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_CenteredText",
        "/Script/SLFConversion.W_Settings_CenteredText",
        clear_all=True
    )

    # Fix W_Settings to use correct parent (UW_Settings instead of UW_Navigable_InputReader)
    # This widget should have been reparented already, just needs correct parent
    fix_widget(
        "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings",
        "/Script/SLFConversion.W_Settings",
        clear_all=True
    )

    # Fix W_Settings_Entry - this one still has Blueprint parent
    # Use full clear since C++ implements all the logic
    fix_widget(
        "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_Entry",
        "/Script/SLFConversion.W_Settings_Entry",
        clear_all=True
    )

    unreal.log_warning("")
    unreal.log_warning("=" * 70)
    unreal.log_warning("Done!")
    unreal.log_warning("=" * 70)


if __name__ == "__main__":
    main()
