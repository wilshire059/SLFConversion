# fix_animbp_variable_names.py
# Rename Blueprint variables to match C++ property names
# This fixes the "will never be taken" transition errors

import unreal

def log(msg):
    print(f"[FixVars] {msg}")
    unreal.log_warning(f"[FixVars] {msg}")

ANIMBP_PATH = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

# Map of old Blueprint variable names to new C++ property names
VARIABLE_RENAMES = {
    "bIsAccelerating?": "bIsAccelerating",
    "bIsBlocking?": "bIsBlocking",
    "bIsFalling?": "bIsFalling",
    "IsResting?": "IsResting",
}

def main():
    log("=" * 80)
    log("FIX ANIMBP VARIABLE NAMES TO MATCH C++ PROPERTIES")
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

    # Use SLFAutomationLibrary.rename_variable function
    renamed_count = 0
    for old_name, new_name in VARIABLE_RENAMES.items():
        # Check if variable exists with old name
        if any(old_name in v for v in variables):
            log(f"Renaming: '{old_name}' -> '{new_name}'")
            result = unreal.SLFAutomationLibrary.rename_variable(bp, old_name, new_name)
            if result:
                log(f"  [OK] Renamed successfully")
                renamed_count += 1
            else:
                log(f"  [SKIP] Variable not found or already named correctly")
        else:
            log(f"Variable '{old_name}' not found in list (may already be renamed)")

    log(f"\nRenamed {renamed_count} variables")

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
    log("\nVerifying variables after rename:")
    variables_after = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
    for var in variables_after:
        log(f"  - {var}")

    # Run diagnosis to check transitions
    log("\nRunning diagnosis to check transitions...")
    diagnosis = unreal.SLFAutomationLibrary.diagnose_anim_bp(bp)

    # Check for "will never be taken" errors
    if "will never be taken" in diagnosis:
        log("[WARNING] Transitions still show 'will never be taken' errors!")
        log("This means the rename did not fix the issue.")
    else:
        log("[SUCCESS] No 'will never be taken' errors found!")

    log("\n[DONE] Variable rename attempt complete")

if __name__ == "__main__":
    main()
