#!/usr/bin/env python3
"""Clear EventGraphs from ABP_SoulslikeEnemy and ABP_SoulslikeBossNew to fix PIE exit errors"""

import unreal

def log(msg):
    print(f"[ClearAnimBP] {msg}")
    unreal.log_warning(f"[ClearAnimBP] {msg}")

ANIMBP_PATHS = [
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeEnemy",
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeBossNew",
]

def main():
    log("=" * 70)
    log("CLEARING EVENTGRAPHS FROM ENEMY ANIMBPS")
    log("=" * 70)

    if not hasattr(unreal, 'SLFAutomationLibrary'):
        log("[ERROR] SLFAutomationLibrary not available!")
        return

    for path in ANIMBP_PATHS:
        log(f"\n--- Processing: {path.split('/')[-1]} ---")

        bp = unreal.EditorAssetLibrary.load_asset(path)
        if not bp:
            log(f"  [ERROR] Could not load: {path}")
            continue

        log(f"  Loaded: {bp.get_name()}")

        # Check current parent
        parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
        log(f"  Parent: {parent}")

        # Clear EventGraph only (keep variables for AnimGraph)
        result = unreal.SLFAutomationLibrary.clear_event_graphs(bp)
        log(f"  clear_event_graphs result: {result}")

        # Compile
        try:
            compile_result = unreal.BlueprintEditorLibrary.compile_blueprint(bp)
            log(f"  Compile: {compile_result}")
        except Exception as e:
            log(f"  Compile error: {e}")

        # Save
        save_result = unreal.EditorAssetLibrary.save_asset(path)
        log(f"  Save: {'SUCCESS' if save_result else 'FAILED'}")

        # Check for errors
        errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
        if "Errors: 0" in errors:
            log("  Status: OK - No compile errors")
        else:
            log(f"  Status: {errors[:200]}")

    log("\n" + "=" * 70)
    log("DONE - EventGraphs cleared, PIE exit errors should be fixed")
    log("=" * 70)

if __name__ == "__main__":
    main()
