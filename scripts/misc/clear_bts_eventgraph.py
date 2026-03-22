# clear_bts_eventgraph.py
# Clear the EventGraph of BTS_TryGetAbility to ensure C++ TickNode runs

import unreal

def log(msg):
    print(f"[CLEAR_BTS] {msg}")

def main():
    log("=" * 60)
    log("Clearing BTS_TryGetAbility EventGraph")
    log("=" * 60)

    bp_path = "/Game/SoulslikeFramework/Blueprints/_AI/Services/BTS_TryGetAbility"

    # Load the Blueprint
    bp = unreal.load_asset(bp_path)
    if not bp:
        log(f"ERROR: Could not load Blueprint at {bp_path}")
        return

    log(f"Blueprint loaded: {bp.get_name()}")

    # Check if we have the automation library
    if hasattr(unreal, 'SLFAutomationLibrary'):
        log("Using SLFAutomationLibrary to clear EventGraph")
        result = unreal.SLFAutomationLibrary.clear_graphs_keep_variables(bp)
        log(f"clear_graphs_keep_variables returned: {result}")

        if result:
            # Save
            saved = unreal.EditorAssetLibrary.save_asset(bp_path, only_if_is_dirty=False)
            log(f"Save result: {saved}")
    else:
        log("SLFAutomationLibrary not available, trying alternative")

        # Try using Kismet's function library
        if hasattr(unreal, 'BlueprintEditorLibrary'):
            # Get all graphs
            try:
                # This might help us see what graphs exist
                uber_graphs = unreal.BlueprintEditorLibrary.find_graph(bp, "EventGraph")
                log(f"EventGraph found: {uber_graphs}")
            except Exception as e:
                log(f"Error finding graphs: {e}")

    log("")
    log("=" * 60)
    log("Done - Please test in PIE")

if __name__ == "__main__":
    main()
