"""
Migrate Settings menu widgets to C++ using the standard pattern:
1. Restore from bp_only (fresh copy)
2. Clear ALL Blueprint logic
3. Reparent to C++
4. Compile and save

Run with:
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/SLFConversion/SLFConversion.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/migrate_settings_widgets.py" ^
  -stdout -unattended -nosplash 2>&1
"""

import unreal

# Widgets to migrate with their C++ parent classes
SETTINGS_WIDGETS = {
    # Parent widgets first (dependency order)
    "/Game/SoulslikeFramework/Widgets/_Generic/W_Navigable": "/Script/SLFConversion.W_Navigable",
    "/Game/SoulslikeFramework/Widgets/_Generic/W_Navigable_InputReader": "/Script/SLFConversion.W_Navigable_InputReader",
    "/Game/SoulslikeFramework/Widgets/_Generic/W_GenericButton": "/Script/SLFConversion.W_GenericButton",

    # Settings widgets
    "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_Entry": "/Script/SLFConversion.W_Settings_Entry",
    "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_CategoryEntry": "/Script/SLFConversion.W_Settings_CategoryEntry",
    "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_CenteredText": "/Script/SLFConversion.W_Settings_CenteredText",
    "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings": "/Script/SLFConversion.W_Settings",
}


def migrate_widget(bp_path, cpp_path):
    """Migrate a single widget using clear + reparent pattern."""
    unreal.log_warning(f"")
    unreal.log_warning(f"=== Migrating {bp_path.split('/')[-1]} ===")

    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_warning(f"  SKIP: Could not load {bp_path}")
        return False

    # Step 1: Clear ALL Blueprint logic
    unreal.log_warning(f"  Step 1: Clearing Blueprint logic...")
    try:
        unreal.SLFAutomationLibrary.clear_all_blueprint_logic_no_compile(bp)
        unreal.log_warning(f"    Cleared")
    except Exception as e:
        unreal.log_warning(f"    Warning: {e}")

    # Save after clearing
    unreal.EditorAssetLibrary.save_asset(bp_path)

    # Reload
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_warning(f"  FAIL: Could not reload after clear")
        return False

    # Step 2: Reparent to C++
    unreal.log_warning(f"  Step 2: Reparenting to {cpp_path.split('.')[-1]}...")
    try:
        ok = unreal.SLFAutomationLibrary.reparent_blueprint(bp, cpp_path)
        if not ok:
            unreal.log_warning(f"    FAIL: Reparent returned false")
            return False
        unreal.log_warning(f"    Reparented")
    except Exception as e:
        unreal.log_warning(f"    Error: {e}")
        return False

    # Step 3: Compile and save
    unreal.log_warning(f"  Step 3: Compiling...")
    try:
        result = unreal.SLFAutomationLibrary.compile_and_save(bp)
        if result:
            unreal.log_warning(f"    SUCCESS")
            return True
        else:
            unreal.log_warning(f"    FAIL: Compile failed")
            return False
    except Exception as e:
        unreal.log_warning(f"    Error: {e}")
        return False


def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("Migrating Settings Widgets to C++")
    unreal.log_warning("=" * 70)

    success_count = 0
    fail_count = 0

    for bp_path, cpp_path in SETTINGS_WIDGETS.items():
        if migrate_widget(bp_path, cpp_path):
            success_count += 1
        else:
            fail_count += 1

    unreal.log_warning("")
    unreal.log_warning("=" * 70)
    unreal.log_warning(f"Results: {success_count} succeeded, {fail_count} failed")
    unreal.log_warning("=" * 70)


if __name__ == "__main__":
    main()
