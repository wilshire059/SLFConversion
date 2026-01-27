# rename_animbp_variables.py
# Rename AnimBP variables to remove "?" suffix, making them match C++ UPROPERTY names
# Run BEFORE reparenting to C++ class

import unreal

def log(msg):
    unreal.log_warning(f"[RENAME] {msg}")

# Variables to rename: old_name -> new_name
VARIABLE_RENAMES = {
    "bIsAccelerating?": "bIsAccelerating",
    "bIsFalling?": "bIsFalling",
    "bIsBlocking?": "bIsBlocking",
    "IsResting?": "IsResting",
}

def main():
    log("=" * 70)
    log("RENAMING ANIMBP VARIABLES (Removing ? suffix)")
    log("=" * 70)

    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

    bp = unreal.load_asset(animbp_path)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    if not hasattr(unreal, 'SLFAutomationLibrary'):
        log("[ERROR] SLFAutomationLibrary not available")
        return

    # Step 1: List current variables
    log("")
    log("=== CURRENT VARIABLES ===")
    current_vars = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
    for v in current_vars:
        log(f"  {v}")

    # Step 2: Rename variables to remove "?" suffix
    log("")
    log("=== RENAMING VARIABLES ===")
    renamed_count = 0
    for old_name, new_name in VARIABLE_RENAMES.items():
        success = unreal.SLFAutomationLibrary.rename_variable(bp, old_name, new_name)
        if success:
            log(f"  [OK] Renamed '{old_name}' -> '{new_name}'")
            renamed_count += 1
        else:
            log(f"  [--] Failed to rename '{old_name}' (may not exist)")

    log(f"  Renamed {renamed_count}/{len(VARIABLE_RENAMES)} variables")

    # Step 3: Compile and save
    log("")
    log("=== COMPILING AND SAVING ===")
    success = unreal.SLFAutomationLibrary.compile_and_save(bp)
    if success:
        log("[OK] Compiled and saved")
    else:
        log("[WARN] Compile may have issues")

    # Step 4: Force save
    save_result = unreal.EditorAssetLibrary.save_asset(animbp_path, only_if_is_dirty=False)
    log(f"Save result: {save_result}")

    # Step 5: Check for errors
    errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
    if errors:
        log("")
        log("=== COMPILE STATUS ===")
        for line in errors.split('\n')[:30]:
            log(line)
    else:
        log("[OK] No compile errors!")

    # Step 6: Final state
    log("")
    log("=== FINAL VARIABLES ===")
    final_vars = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
    for v in final_vars:
        log(f"  {v}")

if __name__ == "__main__":
    main()
