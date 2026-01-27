# fix_animbp_safe.py
# Safely fix the AnimBP:
# 1. Reparent to UABP_SoulslikeCharacter_Additive C++ class
# 2. Rename variables with "?" suffix to remove the "?"
# 3. DO NOT clear EventGraph (causes crash on AnimBP)
import unreal

def log(msg):
    unreal.log_warning(f"[FIXS] {msg}")

def main():
    log("=" * 70)
    log("SAFELY FIXING ANIMBP")
    log("=" * 70)

    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"
    cpp_parent_path = "/Script/SLFConversion.ABP_SoulslikeCharacter_Additive"

    bp = unreal.load_asset(animbp_path)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    if not hasattr(unreal, 'SLFAutomationLibrary'):
        log("[ERROR] SLFAutomationLibrary not available")
        return

    # Step 1: Check current parent
    current_parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
    log(f"Current parent: {current_parent}")

    # Step 2: Reparent to C++ class
    log("")
    log("=== STEP 1: REPARENTING TO C++ CLASS ===")
    log(f"Target parent: {cpp_parent_path}")

    success = unreal.SLFAutomationLibrary.reparent_blueprint(bp, cpp_parent_path)
    if success:
        log("[OK] Reparented successfully")
    else:
        log("[ERROR] Failed to reparent")
        return

    # Verify parent after reparenting
    new_parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
    log(f"New parent: {new_parent}")

    # Step 3: Rename variables with "?" suffix
    log("")
    log("=== STEP 2: RENAMING '?' SUFFIX VARIABLES ===")

    # Map of old names to new names
    rename_map = [
        ("bIsAccelerating?", "bIsAccelerating"),
        ("bIsBlocking?", "bIsBlocking"),
        ("bIsFalling?", "bIsFalling"),
        ("IsResting?", "IsResting"),
        ("IsGuarding?", "IsGuarding"),
        ("IsCrouched?", "IsCrouched"),
    ]

    for old_name, new_name in rename_map:
        # Check if old variable exists
        variables = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
        old_exists = old_name in [str(v) for v in variables]

        if old_exists:
            success = unreal.SLFAutomationLibrary.rename_variable(bp, old_name, new_name)
            if success:
                log(f"  [OK] Renamed {old_name} -> {new_name}")
            else:
                log(f"  [SKIP] Could not rename {old_name} (may conflict with C++ property)")
        else:
            log(f"  [SKIP] {old_name} does not exist")

    # Step 4: Compile and save
    log("")
    log("=== STEP 3: COMPILING AND SAVING ===")
    success = unreal.SLFAutomationLibrary.compile_and_save(bp)
    if success:
        log("[OK] Compiled and saved successfully")
    else:
        log("[WARN] Compile may have warnings - checking errors...")

    # Step 5: Check for errors
    errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
    if errors:
        log("")
        log("=== COMPILE MESSAGES ===")
        for line in errors.split('\n')[:30]:
            log(line)
    else:
        log("[OK] No compile errors!")

    # Step 6: Final state
    log("")
    log("=== FINAL STATE ===")
    final_parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
    log(f"Final parent: {final_parent}")

    final_vars = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
    log(f"Final Blueprint variables: {len(final_vars)}")
    for v in final_vars:
        log(f"  {v}")

    log("")
    log("=== FIX COMPLETE ===")

if __name__ == "__main__":
    main()
