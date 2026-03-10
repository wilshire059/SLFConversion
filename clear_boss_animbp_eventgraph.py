#!/usr/bin/env python3
"""
Clear the EventGraph of ABP_SoulslikeBossNew to fix PoiseBreakAsset errors.

The AnimBP still has Blueprint EventGraph code trying to access AC_AI_CombatManager.
The EventGraph needs to be cleared (variables kept for AnimGraph).
"""

import unreal

def log(msg):
    print(f"[ClearBossAnimBP] {msg}")
    unreal.log_warning(f"[ClearBossAnimBP] {msg}")

def main():
    log("=" * 70)
    log("CLEARING ABP_SoulslikeBossNew EVENTGRAPH")
    log("=" * 70)

    bp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeBossNew"

    log(f"Loading: {bp_path}")
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        log(f"ERROR: Could not load: {bp_path}")
        return

    log(f"Loaded: {bp.get_name()}")

    # Clear ONLY EventGraph (keep AnimGraph and variables)
    log("Clearing ONLY EventGraph (keeping AnimGraph and variables)...")
    try:
        cleared_count = unreal.SLFAutomationLibrary.clear_event_graphs(bp)
        log(f"ClearEventGraphs cleared {cleared_count} nodes")
    except Exception as e:
        log(f"ERROR clearing EventGraph: {e}")
        return

    # Compile and save
    log("Compiling and saving...")
    try:
        compile_result = unreal.SLFAutomationLibrary.compile_and_save(bp)
        log(f"CompileAndSave result: {compile_result}")
    except Exception as e:
        log(f"ERROR compiling: {e}")
        return

    # Check for errors
    try:
        errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
        if errors:
            log(f"Compile errors: {errors}")
        else:
            log("No compile errors - SUCCESS!")
    except Exception as e:
        log(f"Note: {e}")

    log("=" * 70)
    log("DONE")
    log("=" * 70)

if __name__ == "__main__":
    main()
