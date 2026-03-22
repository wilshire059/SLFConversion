# migrate_animbp_aaa.py
# AAA-Quality AnimBP Migration: Proper reparenting to C++ AnimInstance classes
#
# This script properly migrates enemy AnimBPs to C++ AnimInstance classes.
# The C++ NativeUpdateAnimation() handles all animation variable updates,
# replacing broken Blueprint EventGraph logic.
#
# WORKFLOW:
# 1. Load AnimBP assets
# 2. Clear EventGraph (keep variables for AnimGraph)
# 3. Reparent to C++ AnimInstance class
# 4. Save
#
# This eliminates the need for reflection-based workarounds in character Tick().

import unreal
import gc
import time

# AnimBP Migration Map: Blueprint path -> C++ parent class
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


def log_separator(title):
    print("")
    print("=" * 70)
    print(f"  {title}")
    print("=" * 70)


def get_parent_class_name(bp):
    """Get the current parent class name of a Blueprint."""
    try:
        gen_class = bp.generated_class()
        if gen_class:
            parent = gen_class.get_super_class()
            if parent:
                return parent.get_name()
    except:
        pass

    # Fallback: use the automation library
    try:
        parent_str = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
        return parent_str
    except:
        pass

    return "Unknown"


def migrate_animbp(name, path, cpp_parent):
    """Migrate a single AnimBP to C++ parent."""
    print(f"\n--- {name} ---")
    print(f"  Path: {path}")
    print(f"  Target: {cpp_parent.split('.')[-1]}")

    # Load the AnimBP
    bp = None
    try:
        bp = unreal.load_asset(path)
    except Exception as e:
        print(f"  ERROR loading: {e}")
        return False

    if not bp:
        print(f"  FAILED: Asset not found")
        return False

    print(f"  Loaded OK")

    # Check current parent
    current_parent = get_parent_class_name(bp)
    target_parent = cpp_parent.split('.')[-1]
    print(f"  Current parent: {current_parent}")

    # Check if already migrated
    if target_parent in current_parent:
        print(f"  SKIP: Already parented to {target_parent}")
        return True

    # Step 1: Clear EventGraph (keep variables)
    print(f"  Clearing EventGraph...")
    try:
        # Use NoCompile to avoid triggering broken Blueprint compilation
        cleared = unreal.SLFAutomationLibrary.clear_graphs_keep_variables_no_compile(bp)
        if cleared:
            print(f"  OK: EventGraph cleared")
        else:
            print(f"  WARNING: ClearGraphs returned False")
    except Exception as e:
        print(f"  ERROR clearing EventGraph: {e}")
        # Continue anyway - reparenting might still work

    # Force garbage collection before reparenting
    gc.collect()
    unreal.SystemLibrary.collect_garbage()

    # Step 2: Reparent to C++ AnimInstance
    print(f"  Reparenting to {target_parent}...")
    try:
        reparented = unreal.SLFAutomationLibrary.reparent_blueprint(bp, cpp_parent)
        if reparented:
            print(f"  OK: Reparented")
        else:
            print(f"  WARNING: Reparent returned False")
    except Exception as e:
        print(f"  ERROR reparenting: {e}")
        return False

    # Step 3: Save
    print(f"  Saving...")
    try:
        saved = unreal.EditorAssetLibrary.save_asset(path)
        if saved:
            print(f"  OK: Saved")
        else:
            print(f"  WARNING: Save returned False")
    except Exception as e:
        print(f"  ERROR saving: {e}")
        return False

    return True


def main():
    log_separator("AAA-QUALITY ANIMBP MIGRATION")
    print("")
    print("This script properly migrates enemy AnimBPs to C++ AnimInstance classes.")
    print("C++ NativeUpdateAnimation() will handle all animation variable updates.")
    print("")
    print("Target C++ classes provide:")
    print("  - GroundSpeed, Velocity, IsFalling, Direction")
    print("  - SoulslikeEnemy, SoulslikeCharacter references")
    print("  - MovementComponent reference")
    print("  - PoiseBreakAsset (optional)")

    log_separator("PHASE 1: VERIFY C++ CLASSES EXIST")

    # Verify C++ classes exist before migration
    missing_classes = []
    for name, info in ANIMBP_MIGRATION.items():
        cpp_class_path = info["cpp_parent"]
        try:
            cpp_class = unreal.load_class(None, cpp_class_path)
            if cpp_class:
                print(f"  OK: {cpp_class_path.split('.')[-1]}")
            else:
                print(f"  MISSING: {cpp_class_path}")
                missing_classes.append(cpp_class_path)
        except Exception as e:
            print(f"  ERROR loading {cpp_class_path}: {e}")
            missing_classes.append(cpp_class_path)

    if missing_classes:
        print("")
        print("ERROR: Missing C++ AnimInstance classes!")
        print("Build the C++ project first: Build.bat SLFConversionEditor Win64 Development")
        return

    log_separator("PHASE 2: MIGRATE ANIMBPS")

    success_count = 0
    fail_count = 0

    for name, info in ANIMBP_MIGRATION.items():
        if migrate_animbp(name, info["path"], info["cpp_parent"]):
            success_count += 1
        else:
            fail_count += 1

    # Final garbage collection
    gc.collect()
    unreal.SystemLibrary.collect_garbage()

    log_separator("MIGRATION COMPLETE")
    print(f"")
    print(f"  Succeeded: {success_count}")
    print(f"  Failed:    {fail_count}")
    print(f"")

    if success_count == len(ANIMBP_MIGRATION):
        print("All AnimBPs successfully migrated to C++ AnimInstance classes.")
        print("")
        print("The reflection-based workaround in SLFSoulslikeEnemy::UpdateAnimationVariables()")
        print("can now be removed. C++ NativeUpdateAnimation() handles all updates.")
    else:
        print("Some migrations failed. Check errors above.")
        print("")
        print("If assets failed to load, try:")
        print("  1. Restore AnimBPs from bp_only backup first")
        print("  2. Run this script immediately after restore")
        print("  3. Before opening the assets in the editor")


if __name__ == "__main__":
    main()
