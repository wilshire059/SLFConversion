# fix_animbp_complete.py
# Complete fix for AnimBP transitions:
# 1. Remove any Blueprint variables that shadow C++ properties
# 2. Run FixAllBrokenTransitions to recreate VariableGet nodes
# 3. Run RepairTransitionWiring to connect any remaining broken nodes
# 4. Compile and save

import unreal

def log(msg):
    print(f"[FixComplete] {msg}")
    unreal.log_warning(f"[FixComplete] {msg}")

ANIMBP_PATH = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

# Variables that may exist in Blueprint and shadow C++ properties
# (both with and without ? suffix, in case of partial rename)
VARS_TO_REMOVE = [
    "bIsAccelerating",
    "bIsAccelerating?",
    "bIsBlocking",
    "bIsBlocking?",
    "bIsFalling",
    "bIsFalling?",
    "IsResting",
    "IsResting?",
]

def main():
    log("=" * 80)
    log("COMPLETE ANIMBP TRANSITION FIX")
    log("=" * 80)

    # Load the AnimBP
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
    variables = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
    log(f"\nStep 1: Current Blueprint variables ({len(variables)}):")
    for var in variables:
        log(f"  - {var}")

    # Step 2: Remove any shadowing variables
    log("\nStep 2: Removing shadowing Blueprint variables...")
    removed_count = 0
    for var_name in VARS_TO_REMOVE:
        if any(var_name == v or var_name in v for v in variables):
            log(f"  Attempting to remove: '{var_name}'")
            result = unreal.SLFAutomationLibrary.remove_blueprint_variable(bp, var_name)
            if result:
                log(f"    [OK] Removed")
                removed_count += 1
            else:
                log(f"    [SKIP] Not found or could not remove")
    log(f"  Removed {removed_count} variables")

    # Step 3: Run FixBrokenVariableGetNodes to rebuild broken variable references
    log("\nStep 3: Running FixBrokenVariableGetNodes...")
    fixed_nodes = unreal.SLFAutomationLibrary.fix_broken_variable_get_nodes(bp)
    log(f"  Fixed {fixed_nodes} broken VariableGet nodes")

    # Step 4: Run FixAllBrokenTransitions to recreate any remaining broken inputs
    log("\nStep 4: Running FixAllBrokenTransitions...")
    fix_count = unreal.SLFAutomationLibrary.fix_all_broken_transitions(bp)
    log(f"  Fixed {fix_count} broken transition inputs")

    # Step 5: Run RepairTransitionWiring to connect any remaining broken nodes
    log("\nStep 5: Running RepairTransitionWiring...")
    repair_count = unreal.SLFAutomationLibrary.repair_transition_wiring(bp)
    log(f"  Repaired {repair_count} connections")

    # Step 5: Compile the Blueprint
    log("\nStep 5: Compiling Blueprint...")
    unreal.BlueprintEditorLibrary.compile_blueprint(bp)

    # Check errors after compile
    errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
    log(f"Compile result:\n{errors}")

    # Step 6: Save the asset
    log("\nStep 6: Saving asset...")
    unreal.EditorAssetLibrary.save_asset(ANIMBP_PATH)

    # Step 7: Verify - check for "will never be taken" errors
    log("\nStep 7: Verifying transition state...")

    # Get full diagnosis
    diagnosis = unreal.SLFAutomationLibrary.diagnose_anim_bp(bp)

    # Check for problematic transitions
    if "will never be taken" in diagnosis:
        log("[WARNING] Some transitions still show 'will never be taken' warnings!")
        # Count them
        count = diagnosis.count("will never be taken")
        log(f"  Found {count} transitions with warnings")
    else:
        log("[SUCCESS] No 'will never be taken' warnings found!")

    # List final variables
    log("\nFinal Blueprint variables:")
    variables_final = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
    for var in variables_final:
        log(f"  - {var}")

    log("\n[DONE] Complete fix finished")
    log("Please test in PIE to verify animations work correctly")

if __name__ == "__main__":
    main()
