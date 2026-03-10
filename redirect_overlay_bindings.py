#!/usr/bin/env python3
"""Redirect overlay state property bindings from _0 suffixed vars to C++ properties"""

import unreal

def log(msg):
    print(f"[Redirect] {msg}")
    unreal.log_warning(f"[Redirect] {msg}")

ANIMBP_PATH = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

def main():
    log("=" * 70)
    log("REDIRECT OVERLAY STATE PROPERTY BINDINGS")
    log("=" * 70)

    bp = unreal.load_asset(ANIMBP_PATH)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    log(f"Loaded: {bp.get_name()}")

    if not hasattr(unreal, 'SLFAutomationLibrary'):
        log("[ERROR] SLFAutomationLibrary not available")
        return

    # Step 1: First export current state for comparison
    log("\n--- Step 1: Export Current State ---")
    output_path = "C:/scripts/SLFConversion/migration_cache/animbp_before_redirect.txt"
    result = unreal.SLFAutomationLibrary.export_anim_graph_complete(bp, output_path)
    log(f"Exported {len(result)} chars")

    # Step 2: Run the redirect function
    log("\n--- Step 2: Redirect Property Bindings ---")
    redirect_count = unreal.SLFAutomationLibrary.redirect_overlay_state_bindings(bp)
    log(f"Redirected {redirect_count} binding(s)")

    # Step 3: Compile Blueprint
    log("\n--- Step 3: Compile Blueprint ---")
    try:
        compile_result = unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        log(f"Compile result: {compile_result}")
    except Exception as e:
        log(f"Compile error: {e}")

    # Step 4: Save Blueprint
    log("\n--- Step 4: Save Blueprint ---")
    save_result = unreal.EditorAssetLibrary.save_asset(ANIMBP_PATH)
    log(f"Save result: {'SUCCESS' if save_result else 'FAILED'}")

    # Step 5: Export to verify
    log("\n--- Step 5: Export After Redirect ---")
    output_path_after = "C:/scripts/SLFConversion/migration_cache/animbp_after_redirect.txt"
    result_after = unreal.SLFAutomationLibrary.export_anim_graph_complete(bp, output_path_after)
    log(f"Exported {len(result_after)} chars")

    # Step 6: Check for remaining invalid references
    log("\n--- Step 6: Check for Remaining Invalid References ---")
    invalid_found = False
    for line in result_after.split('\n'):
        if "_0" in line and ("OverlayState" in line or "PropertyPath" in line):
            log(f"  [WARN] Possible remaining ref: {line.strip()}")
            invalid_found = True

    if not invalid_found:
        log("  No remaining _0 references found!")

    log("\n" + "=" * 70)
    log("DONE - Check compile output for remaining warnings")
    log("=" * 70)

if __name__ == "__main__":
    main()
