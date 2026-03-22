# fix_property_access_paths.py
# Fix PropertyAccess nodes that reference variables with ? suffix

import unreal

def log(msg):
    print(f"[FixPA] {msg}")
    unreal.log_warning(f"[FixPA] {msg}")

def main():
    log("=" * 70)
    log("FIXING PROPERTYACCESS PATHS")
    log("=" * 70)

    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

    bp = unreal.load_asset(animbp_path)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    if not hasattr(unreal, 'SLFAutomationLibrary'):
        log("[ERROR] SLFAutomationLibrary not available")
        return

    # Define PropertyAccess path fixes
    # Format: (old_path, new_path)
    fixes = [
        ("IsGuarding?", "IsGuarding"),
        ("bIsBlocking?", "bIsBlocking"),
        ("bIsFalling?", "bIsFalling"),
        ("IsResting?", "IsResting"),
        ("bIsAccelerating?", "bIsAccelerating"),
        ("LeftHandOverlayState_0", "LeftHandOverlayState"),
        ("RightHandOverlayState_0", "RightHandOverlayState"),
    ]

    total_fixed = 0

    for old_name, new_name in fixes:
        log(f"  Fixing: {old_name} -> {new_name}")
        fixed = unreal.SLFAutomationLibrary.fix_property_access_paths(bp, old_name, new_name)
        log(f"    Fixed {fixed} references")
        total_fixed += fixed

    log(f"")
    log(f"=== TOTAL FIXED: {total_fixed} ===")

    if total_fixed > 0:
        log("")
        log("=== SAVING ===")
        save_result = unreal.EditorAssetLibrary.save_asset(animbp_path, only_if_is_dirty=False)
        log(f"Save result: {save_result}")

        # Verify
        log("")
        log("=== VERIFYING ===")
        diagnostic = unreal.SLFAutomationLibrary.diagnose_anim_bp(bp)

        # Check for remaining issues
        if "?" in diagnostic:
            log("[WARNING] Still found '?' in diagnostic - some paths may not be fixed")
        else:
            log("[OK] No '?' found in diagnostic")

if __name__ == "__main__":
    main()
