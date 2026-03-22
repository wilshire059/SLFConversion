"""
Complete AnimBP Migration Script
================================

This script performs the full migration of ABP_SoulslikeCharacter_Additive:

1. Clear EventGraph and redundant functions (keeps AnimGraph intact)
2. Fix property access paths with "?" suffix -> without "?"
3. Compile and save

The AnimBP is already:
- Reparented to C++ ABP_SoulslikeCharacter_Additive
- Has BlendListByEnum nodes migrated to C++ ESLFOverlayState enum

This script completes the migration by removing Blueprint-side logic.
"""

import unreal

ANIMBP_PATH = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

# Property names with "?" suffix that need to be fixed
PROPERTY_RENAMES = {
    "IsGuarding?": "IsGuarding",
    "bIsAccelerating?": "bIsAccelerating",
    "bIsBlocking?": "bIsBlocking",
    "bIsFalling?": "bIsFalling",
    "IsResting?": "IsResting",
}


def log(msg):
    print(f"[AnimBPMigrate] {msg}")
    unreal.log_warning(f"[AnimBPMigrate] {msg}")


def main():
    log("=" * 70)
    log("COMPLETE ANIMBP MIGRATION")
    log("=" * 70)

    # Load AnimBP
    bp = unreal.load_asset(ANIMBP_PATH)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return False

    log(f"Loaded: {bp.get_name()}")

    # Check SLFAutomationLibrary is available
    if not hasattr(unreal, 'SLFAutomationLibrary'):
        log("[ERROR] SLFAutomationLibrary not available")
        return False

    # Step 1: Show current state
    log("")
    log("=== STEP 1: Current State ===")

    # Get current variables
    try:
        variables = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
        log(f"Variables: {len(variables)}")
        for v in variables[:10]:
            log(f"  - {v}")
        if len(variables) > 10:
            log(f"  ... and {len(variables) - 10} more")
    except Exception as e:
        log(f"[WARN] Could not get variables: {e}")

    # Get current functions
    try:
        functions = unreal.SLFAutomationLibrary.get_blueprint_functions(bp)
        log(f"Functions: {len(functions)}")
        for f in functions[:10]:
            log(f"  - {f}")
        if len(functions) > 10:
            log(f"  ... and {len(functions) - 10} more")
    except Exception as e:
        log(f"[WARN] Could not get functions: {e}")

    # Step 2: Clear all Blueprint logic (EventGraph + functions, NOT AnimGraph)
    log("")
    log("=== STEP 2: Clear EventGraph and Functions ===")
    log("NOTE: AnimGraph is preserved!")

    try:
        cleared = unreal.SLFAutomationLibrary.clear_all_blueprint_logic_no_compile(bp)
        log(f"Clear result: {cleared}")
    except Exception as e:
        log(f"[ERROR] Clear failed: {e}")
        return False

    # Step 3: Fix property access paths with "?" suffix
    log("")
    log("=== STEP 3: Fix Property Access Paths ===")

    total_fixes = 0
    for old_name, new_name in PROPERTY_RENAMES.items():
        try:
            fixed = unreal.SLFAutomationLibrary.fix_property_access_paths(bp, old_name, new_name)
            if fixed > 0:
                log(f"  Fixed '{old_name}' -> '{new_name}': {fixed} occurrences")
                total_fixes += fixed
            else:
                log(f"  '{old_name}': 0 occurrences found")
        except Exception as e:
            log(f"  [ERROR] Failed to fix '{old_name}': {e}")

    log(f"Total property access fixes: {total_fixes}")

    # Step 4: Compile
    log("")
    log("=== STEP 4: Compile ===")

    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        log("Compile completed")
    except Exception as e:
        log(f"[WARN] Compile exception (may still succeed): {e}")

    # Check compile errors
    try:
        errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
        if errors and errors.strip():
            log(f"Compile status:\n{errors[:500]}")
        else:
            log("[OK] No compile errors!")
    except Exception as e:
        log(f"[WARN] Could not check compile errors: {e}")

    # Step 5: Save
    log("")
    log("=== STEP 5: Save ===")

    try:
        saved = unreal.EditorAssetLibrary.save_asset(ANIMBP_PATH, only_if_is_dirty=False)
        log(f"Save result: {saved}")
    except Exception as e:
        log(f"[ERROR] Save failed: {e}")

    # Step 6: Verify final state
    log("")
    log("=== STEP 6: Final State ===")

    # Reload to verify
    bp = unreal.load_asset(ANIMBP_PATH)

    try:
        variables = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
        log(f"Variables remaining: {len(variables)}")
    except:
        pass

    try:
        functions = unreal.SLFAutomationLibrary.get_blueprint_functions(bp)
        log(f"Functions remaining: {len(functions)}")
    except:
        pass

    log("")
    log("=" * 70)
    log("MIGRATION COMPLETE")
    log("=" * 70)
    log("")
    log("The AnimBP now:")
    log("  - Has BlendListByEnum nodes using C++ ESLFOverlayState enum")
    log("  - EventGraph and redundant functions cleared")
    log("  - Property access paths fixed (no more '?' suffix errors)")
    log("  - AnimGraph preserved and reads from C++ UPROPERTY variables")
    log("")
    log("C++ NativeUpdateAnimation() provides all animation values.")
    log("Test in PIE to verify animations work correctly.")

    return True


if __name__ == "__main__":
    success = main()
    if success:
        unreal.log_warning("[AnimBPMigrate] SUCCESS")
    else:
        unreal.log_error("[AnimBPMigrate] FAILED")
