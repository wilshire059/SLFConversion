#!/usr/bin/env python3
"""Check and clear EventGraphs from ABP_SoulslikeEnemy and ABP_SoulslikeBossNew"""

import unreal

def log(msg):
    print(f"[CheckAnimBP] {msg}")
    unreal.log_warning(f"[CheckAnimBP] {msg}")

ANIMBP_PATHS = [
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeEnemy",
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeBossNew",
]

def main():
    log("=" * 70)
    log("CHECKING AND CLEARING ANIMBP EVENTGRAPHS")
    log("=" * 70)

    if not hasattr(unreal, 'SLFAutomationLibrary'):
        log("[ERROR] SLFAutomationLibrary not available!")
        return

    for path in ANIMBP_PATHS:
        name = path.split('/')[-1]
        log(f"\n--- Processing: {name} ---")

        bp = unreal.EditorAssetLibrary.load_asset(path)
        if not bp:
            log(f"  [ERROR] Could not load: {path}")
            continue

        log(f"  Loaded: {bp.get_name()}")

        # Check current parent
        parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
        log(f"  Parent: {parent}")

        # Check compile status before
        errors_before = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
        log(f"  Compile status before: {errors_before[:200] if len(errors_before) > 200 else errors_before}")

        # Try to get graph count if available
        try:
            # Use the correct function name
            result = unreal.SLFAutomationLibrary.clear_event_graphs(bp)
            log(f"  clear_event_graphs result: {result}")
        except Exception as e:
            log(f"  clear_event_graphs error: {e}")

        # Compile
        try:
            compile_result = unreal.BlueprintEditorLibrary.compile_blueprint(bp)
            log(f"  Compile result: {compile_result}")
        except Exception as e:
            log(f"  Compile error: {e}")

        # Save
        save_result = unreal.EditorAssetLibrary.save_asset(path)
        log(f"  Save: {'SUCCESS' if save_result else 'FAILED'}")

        # Check for errors after
        errors_after = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
        if "Errors: 0" in errors_after:
            log("  Status: OK - No compile errors")
        else:
            log(f"  Status after: {errors_after[:200] if len(errors_after) > 200 else errors_after}")

    log("\n" + "=" * 70)
    log("DONE - Check if EventGraphs were cleared")
    log("=" * 70)

if __name__ == "__main__":
    main()
