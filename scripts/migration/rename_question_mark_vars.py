"""
Rename '?' suffix variables to match C++ property names.

This uses RenameVariable which automatically updates all K2Node_VariableGet
nodes that reference these variables, preserving the AnimGraph wiring.

The '?' suffix variables are Blueprint-side names that shadow C++ properties.
By renaming them to match the C++ UPROPERTY names (without '?'), the AnimGraph
reads directly from C++ properties set by NativeUpdateAnimation().
"""

import unreal

ANIMBP_PATH = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

# Map: Old Blueprint var name -> C++ property name
VARIABLE_RENAMES = {
    "bIsAccelerating?": "bIsAccelerating",
    "bIsBlocking?": "bIsBlocking",
    "bIsFalling?": "bIsFalling",
    "IsResting?": "IsResting",
}


def log(msg):
    print(f"[RenameVars] {msg}")
    unreal.log_warning(f"[RenameVars] {msg}")


def main():
    log("=" * 70)
    log("RENAME '?' SUFFIX VARIABLES TO MATCH C++ PROPERTIES")
    log("=" * 70)

    bp = unreal.load_asset(ANIMBP_PATH)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return False

    log(f"Loaded: {bp.get_name()}")

    # Get current variables
    log("")
    log("=== CURRENT VARIABLES ===")
    try:
        current_vars = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
        for v in current_vars:
            if "?" in v or "Accelerating" in v or "Blocking" in v or "Falling" in v or "Resting" in v:
                log(f"  {v}")
    except Exception as e:
        log(f"[WARN] Could not list variables: {e}")
        current_vars = []

    # Rename variables
    log("")
    log("=== RENAMING VARIABLES ===")

    renamed_count = 0
    for old_name, new_name in VARIABLE_RENAMES.items():
        if old_name in current_vars:
            log(f"  Renaming '{old_name}' -> '{new_name}'...")
            try:
                success = unreal.SLFAutomationLibrary.rename_variable(bp, old_name, new_name)
                if success:
                    log(f"    [OK] Renamed")
                    renamed_count += 1
                else:
                    log(f"    [WARN] RenameVariable returned False")
            except Exception as e:
                log(f"    [ERROR] {e}")
        else:
            log(f"  [--] '{old_name}' not found (may already be renamed)")

    log(f"\nRenamed {renamed_count} variables")

    # Compile
    log("")
    log("=== COMPILE ===")
    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        log("[OK] Compiled")
    except Exception as e:
        log(f"[WARN] Compile: {e}")

    # Check compile errors
    try:
        errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
        will_never_taken = errors.count("will never be taken") if errors else 0
        log(f"'will never be taken' warnings: {will_never_taken}")
    except:
        pass

    # Verify variables after rename
    log("")
    log("=== VARIABLES AFTER RENAME ===")
    try:
        new_vars = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
        for v in new_vars:
            if "Accelerating" in v or "Blocking" in v or "Falling" in v or "Resting" in v:
                log(f"  {v}")
    except:
        pass

    # Save
    log("")
    log("=== SAVE ===")
    try:
        saved = unreal.EditorAssetLibrary.save_asset(ANIMBP_PATH, only_if_is_dirty=False)
        log(f"Save result: {saved}")
    except Exception as e:
        log(f"[ERROR] Save: {e}")

    log("")
    log("=" * 70)
    log("RENAME COMPLETE")
    log("=" * 70)

    return True


if __name__ == "__main__":
    main()
