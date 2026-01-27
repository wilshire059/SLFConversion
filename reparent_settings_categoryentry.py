"""
Reparent W_Settings_CategoryEntry to C++ class.

This Blueprint has duplicate variables/functions that need to be cleared first.

Run with:
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/SLFConversion/SLFConversion.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/reparent_settings_categoryentry.py" ^
  -stdout -unattended -nosplash 2>&1
"""

import unreal

def main():
    unreal.log_warning("=" * 60)
    unreal.log_warning("Reparenting W_Settings_CategoryEntry to C++")
    unreal.log_warning("=" * 60)

    bp_path = "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_CategoryEntry"

    # Load the Blueprint
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_warning(f"FAIL: Could not load {bp_path}")
        return

    unreal.log_warning(f"Loaded Blueprint: {bp.get_name()}")

    # Use SLFAutomationLibrary to clear Blueprint logic and remove variables
    # This should be available from the project
    try:
        if hasattr(unreal, 'SLFAutomationLibrary'):
            unreal.log_warning("Using SLFAutomationLibrary to clear Blueprint...")

            # Clear all event graphs and remove shadowing variables
            cleared = unreal.SLFAutomationLibrary.clear_all_blueprint_logic(bp)  # Clears logic and variables
            unreal.log_warning(f"Cleared Blueprint logic: {cleared}")

            # Compile to apply changes
            unreal.BlueprintEditorLibrary.compile_blueprint(bp)
            unreal.log_warning("Compiled")

            # Now try reparenting
            cpp_class = unreal.load_class(None, "/Script/SLFConversion.W_Settings_CategoryEntry")
            if cpp_class:
                result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, cpp_class)
                unreal.log_warning(f"Reparent result: {result}")

                if result:
                    # Compile and save
                    unreal.BlueprintEditorLibrary.compile_blueprint(bp)
                    unreal.EditorAssetLibrary.save_asset(bp_path)
                    unreal.log_warning("SUCCESS: Reparented and saved!")
                else:
                    unreal.log_warning("FAIL: Reparent returned false")
            else:
                unreal.log_warning("FAIL: Could not load C++ class")
        else:
            unreal.log_warning("SLFAutomationLibrary not found, trying manual approach...")

            # Try to remove variables one by one
            # This requires different approach - not available via standard Python API

            # Just try reparent with force
            cpp_class = unreal.load_class(None, "/Script/SLFConversion.W_Settings_CategoryEntry")
            if cpp_class:
                # First, load and check current parent
                gen_class = bp.generated_class()
                if gen_class:
                    unreal.log_warning(f"Current generated class: {gen_class.get_name()}")
                    unreal.log_warning(f"Current path: {gen_class.get_path_name()}")

                # Try reparent
                result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, cpp_class)
                unreal.log_warning(f"Reparent result: {result}")

    except Exception as e:
        unreal.log_warning(f"ERROR: {e}")

    unreal.log_warning("")
    unreal.log_warning("=" * 60)
    unreal.log_warning("Done")
    unreal.log_warning("=" * 60)

if __name__ == "__main__":
    main()
