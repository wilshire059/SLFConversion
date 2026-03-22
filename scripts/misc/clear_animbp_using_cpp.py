#!/usr/bin/env python3
"""
Clear EventGraphs from AnimBlueprints using C++ SLFAutomationLibrary.

Uses ClearGraphsKeepVariables to preserve AnimGraph while removing Blueprint logic.
"""

import unreal

def log(msg):
    print(f"[ClearAnimBP] {msg}")
    unreal.log_warning(f"[ClearAnimBP] {msg}")

# Blueprints that need their EventGraphs cleared
BLUEPRINTS_TO_CLEAR = [
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeBossNew",
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeEnemy",
    # Restored Blueprints that call C++ with mismatched function signatures
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/B_SequenceActor",
    "/Game/SoulslikeFramework/Blueprints/Global/GI_SoulslikeFramework",
]

def clear_animbp_graphs(bp_path):
    """Clear EventGraph from AnimBP using C++ automation library."""
    log(f"Processing: {bp_path}")

    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        log(f"  ERROR: Could not load: {bp_path}")
        return False

    log(f"  Loaded: {bp.get_name()}")

    # Use C++ automation library
    try:
        # First, clear all event graph nodes including Event nodes
        clear_result = unreal.SLFAutomationLibrary.clear_all_event_graph_nodes(bp_path, True)
        log(f"  ClearAllEventGraphNodes result: {clear_result}")

        # Reload after clearing
        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            log(f"  ERROR: Could not reload after clear")
            return False

        # Compile and save
        compile_result = unreal.SLFAutomationLibrary.compile_and_save(bp)
        log(f"  CompileAndSave result: {compile_result}")

        return compile_result
    except Exception as e:
        log(f"  ERROR: {e}")
        return False

def main():
    log("=" * 70)
    log("CLEARING ANIMBP EVENTGRAPHS USING C++ AUTOMATION")
    log("=" * 70)
    log("")
    log("Using ClearGraphsKeepVariables to preserve AnimGraph variables.")
    log("")

    success_count = 0
    fail_count = 0

    for bp_path in BLUEPRINTS_TO_CLEAR:
        if clear_animbp_graphs(bp_path):
            success_count += 1
        else:
            fail_count += 1

    log("")
    log("=" * 70)
    log(f"DONE - Success: {success_count}, Failed: {fail_count}")
    log("=" * 70)

if __name__ == "__main__":
    main()
