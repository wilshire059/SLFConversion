"""
Clear W_Settings Blueprint EventGraph to fix compile errors.

The W_Settings Blueprint has remaining EventGraph logic that calls functions
that don't exist or have wrong signatures after C++ migration.

Errors being fixed:
- Could not find function "CanNavigate?" in 'W_Settings_C'
- Could not find function "Event PressButton" in 'W_GenericButton_C'
- Could not find function "Event RefreshResolutions" in 'W_Settings_Entry_C'
- Could not find function "Event InitializeKeyMapping" in 'W_Settings_KeyMapping_C'
- Could not find function "Event ResetAllEntriesToDefault" in 'W_Settings_KeyMapping_C'
- Type mismatch: W Generic Button Object Reference is not compatible

Run with:
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/SLFConversion/SLFConversion.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/clear_w_settings_eventgraph.py" ^
  -stdout -unattended -nosplash 2>&1
"""

import unreal

def clear_blueprint_event_graph(blueprint_path):
    """Clear the EventGraph of a Blueprint using SLFAutomationLibrary."""

    unreal.log(f"="*60)
    unreal.log(f"Clearing EventGraph for: {blueprint_path}")
    unreal.log(f"="*60)

    # Load the Blueprint
    bp = unreal.load_asset(blueprint_path)
    if not bp:
        unreal.log_error(f"Could not load Blueprint: {blueprint_path}")
        return False

    unreal.log(f"Blueprint loaded: {bp.get_name()}")

    # Use SLFAutomationLibrary.clear_graphs_keep_variables (clears ALL graphs but keeps variables)
    try:
        if hasattr(unreal, 'SLFAutomationLibrary'):
            unreal.log("Using SLFAutomationLibrary.clear_graphs_keep_variables...")
            result = unreal.SLFAutomationLibrary.clear_graphs_keep_variables(bp)
            unreal.log(f"Clear result: {result}")

            # Compile the Blueprint
            unreal.log("Compiling Blueprint...")
            unreal.BlueprintEditorLibrary.compile_blueprint(bp)

            # Save the Blueprint
            unreal.log("Saving Blueprint...")
            unreal.EditorAssetLibrary.save_asset(blueprint_path)
            unreal.log(f"SUCCESS: Saved {blueprint_path}")
            return True
        else:
            unreal.log_error("SLFAutomationLibrary not found! Trying alternative...")
    except Exception as e:
        unreal.log_error(f"SLFAutomationLibrary.clear_graphs_keep_variables error: {str(e)}")

    # Alternative: Try BlueprintFixerLibrary
    try:
        if hasattr(unreal, 'BlueprintFixerLibrary'):
            unreal.log("Using BlueprintFixerLibrary.clear_event_graph...")
            result = unreal.BlueprintFixerLibrary.clear_event_graph(bp)
            unreal.log(f"BlueprintFixerLibrary result: {result}")

            unreal.BlueprintEditorLibrary.compile_blueprint(bp)
            unreal.EditorAssetLibrary.save_asset(blueprint_path)
            unreal.log(f"SUCCESS: Saved {blueprint_path}")
            return True
    except Exception as e:
        unreal.log_error(f"BlueprintFixerLibrary error: {str(e)}")

    # Alternative: Try PythonBridge
    try:
        if hasattr(unreal, 'PythonBridge'):
            unreal.log("Using PythonBridge.clear_event_graph...")
            result = unreal.PythonBridge.clear_event_graph(bp)
            unreal.log(f"PythonBridge result: {result}")

            unreal.BlueprintEditorLibrary.compile_blueprint(bp)
            unreal.EditorAssetLibrary.save_asset(blueprint_path)
            unreal.log(f"SUCCESS: Saved {blueprint_path}")
            return True
    except Exception as e:
        unreal.log_error(f"PythonBridge error: {str(e)}")

    unreal.log_error("All approaches failed!")
    return False


def main():
    """Main function to clear problematic Settings widget EventGraphs."""

    blueprints_to_clear = [
        "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings",
    ]

    unreal.log("="*60)
    unreal.log("Clearing Settings Widget EventGraphs")
    unreal.log("="*60)

    success_count = 0
    fail_count = 0

    for bp_path in blueprints_to_clear:
        if clear_blueprint_event_graph(bp_path):
            success_count += 1
        else:
            fail_count += 1

    unreal.log("="*60)
    unreal.log(f"EventGraph clearing complete: {success_count} success, {fail_count} failed")
    unreal.log("="*60)


if __name__ == "__main__":
    main()
