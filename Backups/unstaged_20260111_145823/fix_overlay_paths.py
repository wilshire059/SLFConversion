# fix_overlay_paths.py - Fix remaining overlay state property paths
import unreal

def log(msg):
    unreal.log_warning(f"[OVERLAY] {msg}")

def main():
    log("=" * 70)
    log("FIXING OVERLAY STATE PROPERTY PATHS")
    log("=" * 70)

    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"
    bp = unreal.load_asset(animbp_path)
    
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    if not hasattr(unreal, 'SLFAutomationLibrary'):
        log("[ERROR] SLFAutomationLibrary not available")
        return

    # Fix the overlay state property paths
    property_fixes = [
        ("LeftHandOverlayState_0", "LeftHandOverlayState"),
        ("RightHandOverlayState_0", "RightHandOverlayState"),
    ]

    total_fixed = 0
    for old_path, new_path in property_fixes:
        log(f"Fixing '{old_path}' -> '{new_path}'...")
        fixed = unreal.SLFAutomationLibrary.fix_property_access_paths(bp, old_path, new_path)
        log(f"  Fixed {fixed} occurrences")
        total_fixed += fixed

    log(f"Total fixed: {total_fixed}")

    # Compile and save
    log("")
    log("=== COMPILING AND SAVING ===")
    success = unreal.SLFAutomationLibrary.compile_and_save(bp)
    if success:
        log("[OK] Compiled and saved")
    else:
        log("[WARN] Compile may have warnings")

    # Check for remaining errors
    errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
    if errors:
        log("")
        log("REMAINING ISSUES:")
        for line in errors.split('\n')[:20]:
            log(f"  {line}")
    else:
        log("[OK] No compile errors or warnings!")

if __name__ == "__main__":
    main()
