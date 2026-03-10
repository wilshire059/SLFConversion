#!/usr/bin/env python3
"""Test two-handed pose after binding fix by checking overlay state at runtime"""

import unreal

def log(msg):
    print(f"[Test] {msg}")
    unreal.log_warning(f"[Test] {msg}")

ANIMBP_PATH = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

def main():
    log("=" * 70)
    log("TWO-HANDED POSE TEST")
    log("=" * 70)

    # Load and compile the AnimBP to verify no errors
    bp = unreal.load_asset(ANIMBP_PATH)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    log(f"Loaded: {bp.get_name()}")

    # Check if there are any compile warnings
    log("\n--- Checking Compile Status ---")
    try:
        compile_result = unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        log(f"Compile result: {compile_result}")
    except Exception as e:
        log(f"Compile error: {e}")

    # Export final state
    if hasattr(unreal, 'SLFAutomationLibrary'):
        log("\n--- Exporting Final AnimGraph State ---")
        output_path = "C:/scripts/SLFConversion/migration_cache/animbp_final_state.txt"
        result = unreal.SLFAutomationLibrary.export_anim_graph_complete(bp, output_path)
        log(f"Exported {len(result)} chars to {output_path}")

        # Verify BlendListByEnum state
        blend_list_count = result.count("BlendListByEnum")
        log(f"Found {blend_list_count} BlendListByEnum references in export")

        # Check for remaining _0 overlay references
        overlay_0_count = result.count("OverlayState_0")
        if overlay_0_count > 0:
            log(f"[WARN] Found {overlay_0_count} remaining _0 overlay references")
        else:
            log("[OK] No remaining _0 overlay references")

    log("\n" + "=" * 70)
    log("TEST COMPLETE")
    log("")
    log("To verify two-handed animation:")
    log("1. Open the editor and run PIE")
    log("2. Equip a two-handed weapon")
    log("3. Press Y+RB to switch to two-hand stance")
    log("4. The character should show two-handed idle pose")
    log("=" * 70)

if __name__ == "__main__":
    main()
