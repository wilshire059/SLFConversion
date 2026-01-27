# migrate_enemy_animbps_aaa.py
# AAA AnimBP Migration: Reparent, Rename Variables, Clear EventGraph
#
# This script follows the proper AAA migration pattern:
# 1. Reparent AnimBP to C++ AnimInstance class
# 2. Rename Blueprint variables with "_0" suffix to match C++ property names
# 3. Clear EventGraph (preserve AnimGraph)
# 4. NativeUpdateAnimation in C++ handles all variable updates
#
# After migration, AnimGraph K2Node_VariableGet nodes read directly from C++ UPROPERTY.

import unreal
import gc

def log(msg):
    unreal.log_warning(f"[AAA-MIGRATE] {msg}")

# AnimBPs to migrate
ANIMBP_MIGRATION = {
    "ABP_SoulslikeEnemy": {
        "path": "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeEnemy",
        "cpp_parent": "/Script/SLFConversion.ABP_SoulslikeEnemy",
    },
    "ABP_SoulslikeNPC": {
        "path": "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeNPC",
        "cpp_parent": "/Script/SLFConversion.ABP_SoulslikeNPC",
    },
    "ABP_SoulslikeBossNew": {
        "path": "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeBossNew",
        "cpp_parent": "/Script/SLFConversion.ABP_SoulslikeBossNew",
    },
}

# Variables that might get "_0" suffix during reparenting (matching C++ properties)
# These need to be renamed back to match C++ property names
VARIABLE_RENAMES = [
    # ABP_SoulslikeEnemy variables matching C++ UPROPERTY names
    ("SoulslikeCharacter_0", "SoulslikeCharacter"),
    ("MovementComponent_0", "MovementComponent"),
    ("Velocity_0", "Velocity"),
    ("GroundSpeed_0", "GroundSpeed"),
    ("IsFalling_0", "IsFalling"),
    ("EquipmentComponent_0", "EquipmentComponent"),
    ("LocomotionType_0", "LocomotionType"),
    ("PhysicsWeight_0", "PhysicsWeight"),
    ("SoulslikeEnemy_0", "SoulslikeEnemy"),
    ("ACAICombatManager_0", "ACAICombatManager"),
    ("HitLocation_0", "HitLocation"),
    ("Direction_0", "Direction"),
    ("PoiseBroken_0", "PoiseBroken"),
    ("PoiseBreakAsset_0", "PoiseBreakAsset"),
]


def get_blueprint_variables(bp):
    """Get list of Blueprint variable names"""
    if hasattr(unreal, 'SLFAutomationLibrary'):
        return unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
    return []


def rename_variable(bp, old_name, new_name):
    """Rename a Blueprint variable"""
    if hasattr(unreal, 'SLFAutomationLibrary'):
        return unreal.SLFAutomationLibrary.rename_variable(bp, old_name, new_name)
    return False


def clear_event_graphs(bp):
    """Clear EventGraph nodes (preserve AnimGraph)"""
    if hasattr(unreal, 'SLFAutomationLibrary'):
        return unreal.SLFAutomationLibrary.clear_event_graphs(bp)
    return 0


def get_blueprint_parent_class(bp):
    """Get the parent class of the Blueprint"""
    if hasattr(unreal, 'SLFAutomationLibrary'):
        return unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
    return "Unknown"


def get_compile_errors(bp):
    """Get compile errors for the Blueprint"""
    if hasattr(unreal, 'SLFAutomationLibrary'):
        return unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
    return ""


def migrate_single_animbp(name, info):
    """Migrate a single AnimBP to C++ parent"""
    path = info["path"]
    cpp_parent = info["cpp_parent"]
    cpp_class_name = cpp_parent.split(".")[-1]

    log("")
    log("=" * 60)
    log(f"MIGRATING: {name}")
    log("=" * 60)
    log(f"  Path: {path}")
    log(f"  Target C++: {cpp_class_name}")

    try:
        # Step 1: Load AnimBP
        log("")
        log("[1/6] Loading AnimBP...")
        bp = unreal.load_asset(path)
        if not bp:
            log(f"  ERROR: Could not load asset")
            return False

        current_parent = get_blueprint_parent_class(bp)
        log(f"  Current parent: {current_parent}")

        # Check if already reparented
        if cpp_class_name in current_parent:
            log(f"  Already reparented to C++")
        else:
            # Step 2: Reparent to C++ class
            log("")
            log(f"[2/6] Reparenting to {cpp_class_name}...")

            cpp_class = unreal.load_class(None, cpp_parent)
            if not cpp_class:
                log(f"  ERROR: Could not load C++ class: {cpp_parent}")
                return False

            reparented = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, cpp_class)
            if reparented:
                log(f"  OK: Reparented")
            else:
                log(f"  WARNING: Reparent returned False")

            # Garbage collect to clean up
            gc.collect()
            unreal.SystemLibrary.collect_garbage()

        # Step 3: Get current variables (after reparenting)
        log("")
        log("[3/6] Checking variables...")
        current_vars = get_blueprint_variables(bp)
        log(f"  Variables found: {len(current_vars)}")
        for v in current_vars[:10]:
            log(f"    - {v}")
        if len(current_vars) > 10:
            log(f"    ... and {len(current_vars) - 10} more")

        # Step 4: Rename "_0" suffix variables to match C++ properties
        log("")
        log("[4/6] Renaming variables to match C++ properties...")
        renamed_count = 0
        for old_name, new_name in VARIABLE_RENAMES:
            if old_name in current_vars:
                success = rename_variable(bp, old_name, new_name)
                if success:
                    log(f"    OK: {old_name} -> {new_name}")
                    renamed_count += 1
                else:
                    log(f"    WARN: Failed to rename {old_name}")
        log(f"  Renamed {renamed_count} variables")

        # Step 5: Clear EventGraph (preserve AnimGraph)
        log("")
        log("[5/6] Clearing EventGraph (preserving AnimGraph)...")
        cleared = clear_event_graphs(bp)
        log(f"  Cleared {cleared} nodes from EventGraph")

        # Step 6: Compile and save
        log("")
        log("[6/6] Compiling and saving...")

        # Force compile
        if hasattr(unreal, 'SLFAutomationLibrary'):
            compile_result = unreal.SLFAutomationLibrary.compile_and_save(bp)
            log(f"  Compile result: {compile_result}")

        saved = unreal.EditorAssetLibrary.save_asset(path, only_if_is_dirty=False)
        if saved:
            log(f"  OK: Saved")
        else:
            log(f"  WARNING: Save returned False")

        # Verify
        log("")
        log("[VERIFY] Final state:")
        new_parent = get_blueprint_parent_class(bp)
        log(f"  Parent: {new_parent}")

        # Check if reparenting succeeded
        if cpp_class_name not in new_parent:
            log(f"  ERROR: Reparenting failed - parent is still {new_parent}")
            return False

        errors = get_compile_errors(bp)
        # Check for actual errors (not just the word "Error" in the summary)
        # Real errors have patterns like "[Compiler]" or "Error:" at the start
        has_real_errors = False
        if errors:
            for line in errors.split('\n'):
                # Check for actual error indicators, not just "Errors: 0"
                if "[Compiler] Error" in line or "COMPILER ERROR" in line:
                    has_real_errors = True
                    break
            if not has_real_errors:
                log(f"  No compile errors")
            else:
                log(f"  COMPILE ERRORS: {errors[:200]}")

        final_vars = get_blueprint_variables(bp)
        log(f"  Variables: {len(final_vars)}")

        # Success if reparented to correct class
        return True

    except Exception as e:
        log(f"  ERROR: {e}")
        import traceback
        traceback.print_exc()
        return False


def main():
    log("=" * 70)
    log("AAA ENEMY ANIMBP MIGRATION")
    log("=" * 70)
    log("")
    log("Workflow:")
    log("  1. Load AnimBP")
    log("  2. Reparent to C++ AnimInstance class")
    log("  3. Rename '_0' suffix variables to match C++ properties")
    log("  4. Clear EventGraph (preserve AnimGraph)")
    log("  5. Compile and save")
    log("")
    log("Result: AnimGraph reads directly from C++ UPROPERTY via NativeUpdateAnimation")

    success_count = 0
    for name, info in ANIMBP_MIGRATION.items():
        if migrate_single_animbp(name, info):
            success_count += 1

    log("")
    log("=" * 70)
    log(f"RESULT: {success_count}/{len(ANIMBP_MIGRATION)} migrated successfully")
    log("=" * 70)

    if success_count == len(ANIMBP_MIGRATION):
        log("")
        log("All AnimBPs migrated to C++ AnimInstance classes.")
        log("")
        log("C++ NativeUpdateAnimation() provides:")
        log("  - GroundSpeed, Velocity, IsFalling, Direction")
        log("  - SoulslikeEnemy/SoulslikeCharacter references")
        log("  - MovementComponent")
        log("")
        log("AnimGraph reads these C++ UPROPERTY values by name.")
        log("")
        log("NEXT STEP: Remove reflection workaround from SLFSoulslikeEnemy.cpp")


if __name__ == "__main__":
    main()
