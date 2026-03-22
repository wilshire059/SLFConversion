"""
Clear PDA_DayNight function graphs to resolve struct mismatch
Uses SLFAutomationLibrary to clear Blueprint graphs
"""
import unreal

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("CLEARING PDA_DayNight FUNCTION GRAPHS")
    unreal.log_warning("=" * 70)

    bp_path = "/Game/SoulslikeFramework/Data/PDA_DayNight"
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)

    if not bp:
        unreal.log_warning(f"Could not load: {bp_path}")
        return

    unreal.log_warning(f"Processing: {bp.get_name()}")

    # Clear all graphs using SLFAutomationLibrary
    # This keeps the variable definitions but removes all graph logic
    try:
        unreal.SLFAutomationLibrary.clear_graphs_keep_variables_no_compile(bp)
        unreal.log_warning(f"  Cleared graphs (keeping variables)")
    except Exception as e:
        unreal.log_warning(f"  Error clearing graphs: {e}")
        # Try alternative approach
        try:
            unreal.SLFAutomationLibrary.clear_all_blueprint_logic_no_compile(bp)
            unreal.log_warning(f"  Cleared all logic")
        except Exception as e2:
            unreal.log_warning(f"  Alternative also failed: {e2}")

    # Compile
    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        unreal.log_warning(f"  Compiled {bp.get_name()}")
    except Exception as e:
        unreal.log_warning(f"  Compile error: {e}")

    # Save
    unreal.EditorAssetLibrary.save_asset(bp_path)
    unreal.log_warning(f"  Saved {bp_path}")

    unreal.log_warning("Done!")

main()
