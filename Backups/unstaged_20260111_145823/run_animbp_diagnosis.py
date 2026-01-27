# run_animbp_diagnosis.py
# Comprehensive AnimBP diagnosis and fix attempt
import unreal

def log(msg):
    print(msg)
    unreal.log_warning(f"[DIAG] {msg}")

def main():
    log("=" * 70)
    log("ANIMBP COMPREHENSIVE DIAGNOSIS")
    log("=" * 70)

    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

    bp = unreal.load_asset(animbp_path)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    if not hasattr(unreal, 'SLFAutomationLibrary'):
        log("[ERROR] SLFAutomationLibrary not available")
        return

    # 1. Run full diagnosis
    log("")
    log("=== RUNNING FULL DIAGNOSIS ===")
    result = unreal.SLFAutomationLibrary.diagnose_anim_bp(bp)
    for line in result.split('\n'):
        log(line)

    # 2. Inspect ALL transitions to find broken ones
    log("")
    log("=== INSPECTING ALL TRANSITIONS ===")
    result2 = unreal.SLFAutomationLibrary.inspect_transition_graph(bp, "")
    for line in result2.split('\n'):
        log(line)

    # 3. Get compile errors
    log("")
    log("=== COMPILE ERRORS ===")
    errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
    for line in errors.split('\n'):
        log(line)

    # 4. Check if we need to fix property access paths
    log("")
    log("=== ATTEMPTING PROPERTY ACCESS FIXES ===")
    fixes = [
        ("bIsAccelerating?", "bIsAccelerating"),
        ("bIsFalling?", "bIsFalling"),
        ("bIsBlocking?", "bIsBlocking"),
        ("IsResting?", "IsResting"),
        ("Direction(Angle)", "Direction"),
    ]

    total_fixed = 0
    for old_name, new_name in fixes:
        fixed = unreal.SLFAutomationLibrary.fix_property_access_paths(bp, old_name, new_name)
        if fixed > 0:
            log(f"  Fixed {fixed} occurrences of '{old_name}' -> '{new_name}'")
            total_fixed += fixed
        else:
            log(f"  No occurrences of '{old_name}' found")

    if total_fixed > 0:
        log(f"\nTotal fixes applied: {total_fixed}")
        log("Saving AnimBP...")
        unreal.EditorAssetLibrary.save_asset(animbp_path)
        log("Saved!")
    else:
        log("No property access paths needed fixing.")

    log("")
    log("=== DIAGNOSIS COMPLETE ===")

if __name__ == "__main__":
    main()
