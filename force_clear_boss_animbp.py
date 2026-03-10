#!/usr/bin/env python3
"""
Force clear ALL Blueprint logic from ABP_SoulslikeBossNew (except AnimGraph).
Uses ClearAllEventGraphNodes which removes ALL event nodes including interface implementations.
"""

import unreal

def log(msg):
    print(f"[ForceClearBossAnimBP] {msg}")
    unreal.log_warning(f"[ForceClearBossAnimBP] {msg}")

def main():
    log("=" * 70)
    log("FORCE CLEARING ABP_SoulslikeBossNew")
    log("=" * 70)

    bp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeBossNew"

    log(f"Loading: {bp_path}")
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        log(f"ERROR: Could not load: {bp_path}")
        return

    log(f"Loaded: {bp.get_name()}")

    # Use ClearAllEventGraphNodes which is more aggressive
    log("")
    log("Clearing ALL EventGraph nodes (including Event nodes)...")
    try:
        result = unreal.SLFAutomationLibrary.clear_all_event_graph_nodes(bp_path, True)
        log(f"ClearAllEventGraphNodes result: {result}")
    except Exception as e:
        log(f"ERROR: {e}")
        # Fallback to regular clear
        log("Falling back to ClearEventGraphs...")
        try:
            cleared = unreal.SLFAutomationLibrary.clear_event_graphs(bp)
            log(f"ClearEventGraphs cleared {cleared} nodes")
        except Exception as e2:
            log(f"Fallback error: {e2}")

    # Reload and compile
    log("")
    log("Reloading Blueprint...")
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        log("ERROR: Could not reload Blueprint")
        return

    log("Compiling and saving...")
    try:
        compile_result = unreal.SLFAutomationLibrary.compile_and_save(bp)
        log(f"CompileAndSave result: {compile_result}")
    except Exception as e:
        log(f"Compile error: {e}")

    # Check for errors
    try:
        errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
        log(f"Compile errors: {errors if errors else 'None'}")
    except Exception as e:
        log(f"Note: {e}")

    # Check if still has logic
    try:
        has_logic = unreal.SLFAutomationLibrary.has_event_graph_logic(bp)
        log(f"HasEventGraphLogic after clear: {has_logic}")
    except Exception as e:
        log(f"Note: {e}")

    log("")
    log("=" * 70)
    log("DONE")
    log("=" * 70)

if __name__ == "__main__":
    main()
