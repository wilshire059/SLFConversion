"""
Reparent W_Settings and W_Settings_CategoryEntry to C++ classes.

These widgets were not reparented during migration, causing:
- Cast<UW_Settings_CategoryEntry> to fail
- Tab clicking not working
- C++ NativeConstruct not called

Run with:
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/SLFConversion/SLFConversion.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/reparent_settings_widgets.py" ^
  -stdout -unattended -nosplash 2>&1
"""

import unreal

# Settings widgets that need reparenting - core widgets first
SETTINGS_WIDGETS = [
    # Core settings widgets (must be reparented first)
    ("/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_CategoryEntry", "/Script/SLFConversion.W_Settings_CategoryEntry"),
    ("/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_Entry", "/Script/SLFConversion.W_Settings_Entry"),
    # Main settings widget
    ("/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings", "/Script/SLFConversion.W_Settings"),
]

def reparent_widget(bp_path, new_parent_path):
    """Reparent a Widget Blueprint to a new C++ parent class."""
    unreal.log_warning(f"Reparenting: {bp_path}")

    # Load the Blueprint
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_warning(f"  SKIP: Could not load {bp_path}")
        return False

    # Load the new parent class
    new_parent = unreal.load_class(None, new_parent_path)
    if not new_parent:
        unreal.log_warning(f"  SKIP: Could not load parent class {new_parent_path}")
        return False

    unreal.log_warning(f"  New parent class: {new_parent.get_name()}")

    # Reparent using BlueprintEditorLibrary
    try:
        result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, new_parent)
        if result:
            unreal.log_warning(f"  SUCCESS: Reparented to {new_parent.get_name()}")

            # Compile
            unreal.BlueprintEditorLibrary.compile_blueprint(bp)
            unreal.log_warning(f"  Compiled")

            # Save
            unreal.EditorAssetLibrary.save_asset(bp_path)
            unreal.log_warning(f"  Saved")

            return True
        else:
            unreal.log_warning(f"  FAIL: Reparent returned false")
            return False
    except Exception as e:
        unreal.log_warning(f"  ERROR: {e}")
        return False

def main():
    unreal.log_warning("=" * 60)
    unreal.log_warning("Reparenting Settings Widgets to C++")
    unreal.log_warning("=" * 60)

    success_count = 0
    fail_count = 0
    skip_count = 0

    for bp_path, parent_path in SETTINGS_WIDGETS:
        # Check if already reparented
        bp = unreal.load_asset(bp_path)
        if not bp:
            unreal.log_warning(f"SKIP: {bp_path} - Asset not found")
            skip_count += 1
            continue

        # Check if C++ class exists
        cpp_class = unreal.load_class(None, parent_path)
        if not cpp_class:
            unreal.log_warning(f"SKIP: {bp_path} - C++ class {parent_path} not found")
            skip_count += 1
            continue

        # Reparent
        if reparent_widget(bp_path, parent_path):
            success_count += 1
        else:
            fail_count += 1

    unreal.log_warning("")
    unreal.log_warning("=" * 60)
    unreal.log_warning(f"RESULTS: {success_count} succeeded, {fail_count} failed, {skip_count} skipped")
    unreal.log_warning("=" * 60)

    if success_count > 0:
        unreal.log_warning("")
        unreal.log_warning("Next steps:")
        unreal.log_warning("1. Run PIE test to verify tab clicking works")
        unreal.log_warning("2. Verify Settings menu opens and tabs can be selected")

if __name__ == "__main__":
    main()
