# fix_animbp_final.py
# Fix remaining Property Access paths with _0 suffix
import unreal

def log(msg):
    unreal.log_warning(f"[FINAL] {msg}")

def main():
    log("=" * 70)
    log("FIXING REMAINING PROPERTY ACCESS PATHS")
    log("=" * 70)

    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

    bp = unreal.load_asset(animbp_path)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    if not hasattr(unreal, 'SLFAutomationLibrary'):
        log("[ERROR] SLFAutomationLibrary not available")
        return

    # Fix Property Access paths with _0 suffix
    property_renames = [
        ("LeftHandOverlayState_0", "LeftHandOverlayState"),
        ("RightHandOverlayState_0", "RightHandOverlayState"),
        ("bIsAccelerating_0", "bIsAccelerating"),
    ]

    total_fixed = 0
    for old_name, new_name in property_renames:
        log(f"Fixing '{old_name}' -> '{new_name}'...")
        fixed = unreal.SLFAutomationLibrary.fix_property_access_paths(bp, old_name, new_name)
        log(f"  Fixed {fixed} occurrences")
        total_fixed += fixed

    log(f"Total fixed: {total_fixed}")

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

if __name__ == "__main__":
    main()
