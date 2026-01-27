# fix_animbp_variables.py
# Replace Blueprint _0 suffix variables with C++ properties
import unreal

def log(msg):
    unreal.log_warning(f"[FIXVAR] {msg}")

def main():
    log("=" * 70)
    log("REPLACING _0 SUFFIX VARIABLES WITH C++ PROPERTIES")
    log("=" * 70)

    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

    bp = unreal.load_asset(animbp_path)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    if not hasattr(unreal, 'SLFAutomationLibrary'):
        log("[ERROR] SLFAutomationLibrary not available")
        return

    # Replace variable references from _0 suffix to C++ property names
    replacements = [
        ("bIsAccelerating_0", "bIsAccelerating"),
        ("LeftHandOverlayState_0", "LeftHandOverlayState"),
        ("RightHandOverlayState_0", "RightHandOverlayState"),
    ]

    total_replaced = 0
    for old_name, new_name in replacements:
        log(f"Replacing '{old_name}' -> '{new_name}'...")
        replaced = unreal.SLFAutomationLibrary.replace_variable_references(bp, old_name, new_name)
        log(f"  Replaced {replaced} occurrences")
        total_replaced += replaced

    log(f"Total replaced: {total_replaced}")

    # Remove the _0 suffix variables if they exist
    log("")
    log("=== REMOVING _0 SUFFIX VARIABLES ===")
    for old_name, _ in replacements:
        success = unreal.SLFAutomationLibrary.remove_variable(bp, old_name)
        if success:
            log(f"  [OK] Removed {old_name}")
        else:
            log(f"  [SKIP] {old_name} - could not remove or doesn't exist")

    # Compile and save
    log("")
    log("=== COMPILING AND SAVING ===")
    success = unreal.SLFAutomationLibrary.compile_and_save(bp)
    if success:
        log("[OK] Compiled and saved successfully")
    else:
        log("[WARN] Compile may have warnings")

    # Check for errors
    errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
    if errors:
        log("")
        log("=== COMPILE MESSAGES ===")
        for line in errors.split('\n')[:20]:
            log(line)
    else:
        log("[OK] No compile errors!")

    # Final state
    log("")
    log("=== FINAL STATE ===")
    final_vars = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
    log(f"Final Blueprint variables: {len(final_vars)}")
    for v in final_vars:
        log(f"  {v}")

if __name__ == "__main__":
    main()
