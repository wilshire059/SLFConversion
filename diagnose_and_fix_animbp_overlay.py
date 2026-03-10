#!/usr/bin/env python3
"""Diagnose and fix AnimBP overlay state variable issues"""

import unreal

def log(msg):
    print(f"[AnimBP Fix] {msg}")
    unreal.log_warning(f"[AnimBP Fix] {msg}")

ANIMBP_PATH = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

def main():
    log("=" * 70)
    log("DIAGNOSE AND FIX ANIMBP OVERLAY STATE")
    log("=" * 70)

    bp = unreal.load_asset(ANIMBP_PATH)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    log(f"Loaded: {bp.get_name()}")

    # Check if SLFAutomationLibrary is available
    if not hasattr(unreal, 'SLFAutomationLibrary'):
        log("[ERROR] SLFAutomationLibrary not available")
        return

    # Step 1: Get current Blueprint variables
    log("\n--- Step 1: Get Current Variables ---")
    vars_list = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
    log(f"Variables: {vars_list}")

    # Step 2: Check for shadowing variables
    log("\n--- Step 2: Check for Shadowing Variables ---")
    shadowing_vars = []
    for var in vars_list:
        if "_0" in var and "OverlayState" in var:
            shadowing_vars.append(var)
            log(f"  FOUND SHADOWING: {var}")

    if not shadowing_vars:
        log("  No shadowing _0 variables found")

    # Step 3: Get parent class info
    log("\n--- Step 3: Get Parent Class ---")
    parent_class = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
    log(f"Parent class: {parent_class}")

    # Step 4: Remove shadowing variables if present
    if shadowing_vars:
        log("\n--- Step 4: Remove Shadowing Variables ---")
        for var_name in shadowing_vars:
            log(f"  Removing: {var_name}")
            result = unreal.SLFAutomationLibrary.remove_variable(bp, var_name)
            log(f"    Result: {'SUCCESS' if result else 'FAILED'}")

    # Step 5: Force compile the Blueprint
    log("\n--- Step 5: Compile Blueprint ---")
    try:
        compile_result = unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        log(f"Compile result: {compile_result}")
    except Exception as e:
        log(f"Compile error: {e}")

    # Step 6: Get variables again to confirm
    log("\n--- Step 6: Verify Variables After ---")
    vars_after = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
    log(f"Variables after: {vars_after}")

    # Step 7: Export AnimGraph node info
    log("\n--- Step 7: Export AnimGraph State ---")
    output_path = "C:/scripts/SLFConversion/migration_cache/animbp_overlay_diagnosis.txt"
    result = unreal.SLFAutomationLibrary.export_anim_graph_complete(bp, output_path)
    log(f"Exported {len(result)} chars to {output_path}")

    # Step 8: Save the Blueprint
    log("\n--- Step 8: Save Blueprint ---")
    save_result = unreal.EditorAssetLibrary.save_asset(ANIMBP_PATH)
    log(f"Save result: {'SUCCESS' if save_result else 'FAILED'}")

    # Step 9: Search for BlendListByEnum nodes in the export
    log("\n--- Step 9: BlendListByEnum Analysis ---")
    lines = result.split('\n')
    in_blend_section = False
    blend_info = []
    for line in lines:
        if "BlendListByEnum" in line:
            in_blend_section = True
            blend_info = []
        if in_blend_section:
            blend_info.append(line)
            if "ActiveEnumValue" in line:
                log(f"  {line.strip()}")
            if "LinkedTo" in line:
                log(f"  {line.strip()}")
            if len(blend_info) > 20:
                in_blend_section = False

    log("\n" + "=" * 70)
    log("DONE - Check migration_cache/animbp_overlay_diagnosis.txt for full details")
    log("=" * 70)

if __name__ == "__main__":
    main()
