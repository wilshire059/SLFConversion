"""
Fix remaining AnimBP issues:
1. Reparent to C++ ABP_SoulslikeCharacter_Additive
2. Remove "?" suffix Blueprint variables
"""

import unreal

ANIMBP_PATH = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"
CPP_PARENT = "/Script/SLFConversion.ABP_SoulslikeCharacter_Additive"

# Variables with "?" suffix that shadow C++ properties
VARS_TO_REMOVE = [
    "bIsAccelerating?",
    "bIsBlocking?",
    "bIsFalling?",
    "IsResting?",
    "Direction(Angle)",  # Also problematic - C++ has 'Direction'
]


def log(msg):
    print(f"[FixRemaining] {msg}")
    unreal.log_warning(f"[FixRemaining] {msg}")


def main():
    log("=" * 70)
    log("FIX REMAINING ANIMBP ISSUES")
    log("=" * 70)

    bp = unreal.load_asset(ANIMBP_PATH)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return False

    log(f"Loaded: {bp.get_name()}")

    # Check current parent
    try:
        current_parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
        log(f"Current parent: {current_parent}")
    except:
        current_parent = "Unknown"

    # Step 1: Reparent to C++
    log("")
    log("=== STEP 1: Reparent to C++ ===")

    if "ABP_SoulslikeCharacter_Additive" not in current_parent:
        log(f"Reparenting to {CPP_PARENT}...")
        try:
            success = unreal.SLFAutomationLibrary.reparent_blueprint(bp, CPP_PARENT)
            if success:
                log("[OK] Reparented")
            else:
                log("[WARN] Reparent returned False")
        except Exception as e:
            log(f"[ERROR] Reparent failed: {e}")
    else:
        log("[OK] Already parented to C++")

    # Step 2: Remove "?" variables
    log("")
    log("=== STEP 2: Remove '?' Suffix Variables ===")

    # Get current variables
    try:
        variables = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
        log(f"Current variables: {len(variables)}")
    except:
        variables = []

    removed_count = 0
    for var_name in VARS_TO_REMOVE:
        if var_name in variables:
            log(f"  Removing: {var_name}")
            try:
                result = unreal.SLFAutomationLibrary.remove_variable(bp, var_name)
                if result:
                    log(f"    [OK] Removed")
                    removed_count += 1
                else:
                    log(f"    [SKIP] Could not remove")
            except Exception as e:
                log(f"    [ERROR] {e}")
        else:
            log(f"  [--] {var_name} not found")

    log(f"Removed {removed_count} variables")

    # Step 3: Compile
    log("")
    log("=== STEP 3: Compile ===")

    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        log("[OK] Compile completed")
    except Exception as e:
        log(f"[WARN] Compile exception: {e}")

    # Check errors
    try:
        errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
        if errors and "No compile errors" not in errors and "UP TO DATE" not in errors:
            log(f"Compile status:\n{errors[:400]}")
        else:
            log("[OK] No compile errors")
    except:
        pass

    # Step 4: Save
    log("")
    log("=== STEP 4: Save ===")

    try:
        saved = unreal.EditorAssetLibrary.save_asset(ANIMBP_PATH, only_if_is_dirty=False)
        log(f"Save result: {saved}")
    except Exception as e:
        log(f"[ERROR] Save failed: {e}")

    # Step 5: Verify
    log("")
    log("=== STEP 5: Verify ===")

    bp = unreal.load_asset(ANIMBP_PATH)

    try:
        parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
        log(f"Final parent: {parent}")
    except:
        pass

    try:
        variables = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
        log(f"Variables remaining: {len(variables)}")
        for v in variables[:10]:
            log(f"  - {v}")
    except:
        pass

    log("")
    log("=" * 70)
    log("FIX COMPLETE")
    log("=" * 70)

    return True


if __name__ == "__main__":
    success = main()
    if success:
        unreal.log_warning("[FixRemaining] SUCCESS")
    else:
        unreal.log_error("[FixRemaining] FAILED")
