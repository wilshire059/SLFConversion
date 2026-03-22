"""
Reparent W_Settings_CategoryEntry to C++ using SLFAutomationLibrary.

Run with:
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/SLFConversion/SLFConversion.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/reparent_categoryentry_cpp.py" ^
  -stdout -unattended -nosplash 2>&1
"""

import unreal

def main():
    unreal.log_warning("=" * 60)
    unreal.log_warning("Reparent W_Settings_CategoryEntry using C++ Library")
    unreal.log_warning("=" * 60)

    bp_path = "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_CategoryEntry"
    cpp_class_path = "/Script/SLFConversion.W_Settings_CategoryEntry"

    # Load the Blueprint
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_warning(f"FAIL: Could not load {bp_path}")
        return

    unreal.log_warning(f"Loaded Blueprint: {bp.get_name()}")

    # First, clear Blueprint logic using SLFAutomationLibrary
    if hasattr(unreal, 'SLFAutomationLibrary'):
        unreal.log_warning("Step 1: Clear Blueprint logic...")
        cleared = unreal.SLFAutomationLibrary.clear_all_blueprint_logic(bp)
        unreal.log_warning(f"Cleared: {cleared}")

        unreal.log_warning("Step 2: Reparent using C++ function...")
        # Use the C++ ReparentBlueprint function which directly sets ParentClass
        result = unreal.SLFAutomationLibrary.reparent_blueprint(bp, cpp_class_path)
        unreal.log_warning(f"Reparent result: {result}")

        if result:
            unreal.log_warning("Step 3: Compile Blueprint...")
            unreal.BlueprintEditorLibrary.compile_blueprint(bp)
            unreal.log_warning("Compiled")

            unreal.log_warning("Step 4: Save asset...")
            unreal.EditorAssetLibrary.save_asset(bp_path)
            unreal.log_warning("Saved")

            unreal.log_warning("SUCCESS: W_Settings_CategoryEntry reparented to C++!")
        else:
            unreal.log_warning("FAIL: Reparent returned false")
    else:
        unreal.log_warning("FAIL: SLFAutomationLibrary not available")

    unreal.log_warning("")
    unreal.log_warning("=" * 60)
    unreal.log_warning("Done")
    unreal.log_warning("=" * 60)

if __name__ == "__main__":
    main()
