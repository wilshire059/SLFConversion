#!/usr/bin/env python3
"""
Clear EventGraphs from AnimBlueprints that have stale property accesses.

The AnimBP EventGraphs have logic that tries to access Blueprint variables
that no longer exist (because they're now C++ properties). The AnimGraph
reads C++ properties correctly, but the EventGraph still tries to access
the old Blueprint variables.

This script clears ONLY the EventGraph, preserving:
- Variables (AnimGraph reads them)
- AnimGraph (the actual animation state machine)
"""

import unreal

def log(msg):
    print(f"[ClearAnimBPEventGraphs] {msg}")
    unreal.log_warning(f"[ClearAnimBPEventGraphs] {msg}")

# AnimBPs that need their EventGraphs cleared
ANIMBPS_TO_CLEAR = [
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeBossNew",
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeEnemy",
]

def clear_eventgraph_only(bp_path):
    """Clear only the EventGraph from an AnimBlueprint, keep variables and AnimGraph."""
    log(f"Processing: {bp_path}")

    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        log(f"  ERROR: Could not load: {bp_path}")
        return False

    log(f"  Loaded: {bp.get_name()}")

    # Check if this is an AnimBlueprint
    if not isinstance(bp, unreal.AnimBlueprint):
        log(f"  ERROR: Not an AnimBlueprint: {type(bp)}")
        return False

    # Get the EventGraph and clear it
    cleared_nodes = 0

    if hasattr(bp, 'uber_graphs'):
        for graph in bp.uber_graphs:
            graph_name = graph.get_name() if hasattr(graph, 'get_name') else str(graph)
            log(f"  Found graph: {graph_name}")

            if 'EventGraph' in graph_name:
                # Clear nodes from EventGraph
                if hasattr(graph, 'nodes'):
                    nodes_to_remove = list(graph.nodes)
                    for node in nodes_to_remove:
                        if hasattr(graph, 'remove_node'):
                            try:
                                graph.remove_node(node)
                                cleared_nodes += 1
                            except:
                                pass
                log(f"  Cleared {cleared_nodes} nodes from {graph_name}")

    # Try to compile
    if hasattr(unreal, 'KismetEditorLibrary'):
        try:
            unreal.KismetEditorLibrary.compile_blueprint(bp)
            log(f"  Compiled")
        except:
            pass

    if hasattr(unreal, 'BlueprintEditorLibrary'):
        try:
            unreal.BlueprintEditorLibrary.compile_blueprint(bp)
            log(f"  Compiled via BlueprintEditorLibrary")
        except:
            pass

    # Save
    if unreal.EditorAssetLibrary.save_asset(bp_path, only_if_is_dirty=False):
        log(f"  Saved")
        return True
    else:
        log(f"  ERROR: Failed to save")
        return False

def main():
    log("=" * 70)
    log("CLEARING EVENTGRAPHS FROM ANIMBPS")
    log("=" * 70)
    log("")
    log("This preserves variables and AnimGraph, only clears EventGraph nodes.")
    log("")

    success_count = 0
    fail_count = 0

    for bp_path in ANIMBPS_TO_CLEAR:
        if clear_eventgraph_only(bp_path):
            success_count += 1
        else:
            fail_count += 1

    log("")
    log("=" * 70)
    log(f"DONE - Success: {success_count}, Failed: {fail_count}")
    log("=" * 70)

if __name__ == "__main__":
    main()
