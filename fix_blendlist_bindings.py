#!/usr/bin/env python3
"""Fix BlendListByEnum property bindings to use C++ overlay state properties"""

import unreal

def log(msg):
    print(f"[Fix Bindings] {msg}")
    unreal.log_warning(f"[Fix Bindings] {msg}")

ANIMBP_PATH = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

def main():
    log("=" * 70)
    log("FIX BLENDLISTBYENUM PROPERTY BINDINGS")
    log("=" * 70)

    bp = unreal.load_asset(ANIMBP_PATH)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    log(f"Loaded: {bp.get_name()}")

    if not hasattr(unreal, 'SLFAutomationLibrary'):
        log("[ERROR] SLFAutomationLibrary not available")
        return

    # Step 1: Get current variables (should NOT have _0 versions)
    log("\n--- Step 1: Current Variables ---")
    vars_list = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
    log(f"Variables: {vars_list}")

    # Step 2: Fix all BlendListByEnum bindings
    log("\n--- Step 2: Fix BlendListByEnum Bindings ---")
    fix_count = unreal.SLFAutomationLibrary.fix_all_blend_list_by_enum_bindings(bp)
    log(f"Fixed {fix_count} BlendListByEnum node(s)")

    # Step 3: Compile Blueprint
    log("\n--- Step 3: Compile Blueprint ---")
    try:
        compile_result = unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        log(f"Compile result: {compile_result}")
    except Exception as e:
        log(f"Compile error: {e}")

    # Step 4: Export AnimGraph to verify
    log("\n--- Step 4: Export AnimGraph State ---")
    output_path = "C:/scripts/SLFConversion/migration_cache/animbp_after_binding_fix.txt"
    result = unreal.SLFAutomationLibrary.export_anim_graph_complete(bp, output_path)
    log(f"Exported {len(result)} chars")

    # Step 5: Save Blueprint
    log("\n--- Step 5: Save Blueprint ---")
    save_result = unreal.EditorAssetLibrary.save_asset(ANIMBP_PATH)
    log(f"Save result: {'SUCCESS' if save_result else 'FAILED'}")

    # Step 6: Look for binding info in export
    log("\n--- Step 6: Check Bindings in Export ---")
    for line in result.split('\n'):
        if "ActiveEnumValue" in line or "PropertyBinding" in line or "LeftHandOverlayState" in line or "RightHandOverlayState" in line:
            log(f"  {line.strip()}")

    log("\n" + "=" * 70)
    log("DONE - Bindings should now use C++ properties")
    log("=" * 70)

if __name__ == "__main__":
    main()
