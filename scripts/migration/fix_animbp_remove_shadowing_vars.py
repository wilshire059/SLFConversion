# fix_animbp_remove_shadowing_vars.py
# Remove Blueprint variables that shadow C++ properties
# After removal, the transition graphs should automatically bind to C++ properties

import unreal

def log(msg):
    print(f"[FixVars] {msg}")
    unreal.log_warning(f"[FixVars] {msg}")

ANIMBP_PATH = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

# Blueprint variables with "?" suffix that shadow C++ properties
VARS_TO_REMOVE = [
    "bIsAccelerating?",
    "bIsBlocking?",
    "bIsFalling?",
    "IsResting?",
]

def main():
    log("=" * 80)
    log("REMOVE SHADOWING BLUEPRINT VARIABLES FROM ANIMBP")
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

    # Get current variables
    variables = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
    log(f"Current variables ({len(variables)}):")
    for var in variables:
        log(f"  - {var}")

    # Remove the shadowing variables
    removed_count = 0
    for var_name in VARS_TO_REMOVE:
        if any(var_name in v for v in variables):
            log(f"Removing: '{var_name}'")
            result = unreal.SLFAutomationLibrary.remove_blueprint_variable(bp, var_name)
            if result:
                log(f"  [OK] Removed successfully")
                removed_count += 1
            else:
                log(f"  [FAIL] Could not remove")
        else:
            log(f"Variable '{var_name}' not found (may already be removed)")

    log(f"\nRemoved {removed_count} variables")

    # Compile the Blueprint
    log("\nCompiling Blueprint...")
    unreal.BlueprintEditorLibrary.compile_blueprint(bp)

    # Check errors after compile
    errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
    log(f"Compile result:\n{errors}")

    # Save the asset
    log("\nSaving asset...")
    unreal.EditorAssetLibrary.save_asset(ANIMBP_PATH)

    # Verify the changes
    log("\nVerifying variables after removal:")
    variables_after = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
    for var in variables_after:
        log(f"  - {var}")

    # Run diagnosis to check transitions
    log("\nRunning diagnosis to check transitions...")
    diagnosis = unreal.SLFAutomationLibrary.diagnose_anim_bp(bp)

    # Check for "will never be taken" errors
    if "will never be taken" in diagnosis:
        log("[WARNING] Transitions still show 'will never be taken' errors!")
        log("The nodes may need to be reconnected to C++ properties.")
    else:
        log("[SUCCESS] No 'will never be taken' errors found!")

    log("\n[DONE] Variable removal complete")

if __name__ == "__main__":
    main()
