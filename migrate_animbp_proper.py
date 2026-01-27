# migrate_animbp_proper.py
# AAA AnimBP Migration: Clear EventGraph, Remove Variables, Reparent to C++
#
# The correct workflow for AnimBP reparenting:
# 1. Clear EventGraph (broken Blueprint logic)
# 2. Remove ALL Blueprint variables (they conflict with C++ properties)
# 3. Reparent to C++ AnimInstance class
# 4. Save
#
# After migration, C++ NativeUpdateAnimation() provides all animation variables.
# AnimGraph Property Access nodes find C++ properties by name.

import unreal
import gc

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


def main():
    print("=" * 70)
    print("AAA ANIMBP MIGRATION")
    print("=" * 70)
    print("")
    print("Workflow:")
    print("  1. Clear EventGraph (broken Blueprint logic)")
    print("  2. Remove ALL Blueprint variables (conflict with C++ properties)")
    print("  3. Reparent to C++ AnimInstance class")
    print("  4. Save")
    print("")

    success_count = 0

    for name, info in ANIMBP_MIGRATION.items():
        path = info["path"]
        cpp_parent = info["cpp_parent"]
        cpp_class_name = cpp_parent.split(".")[-1]

        print(f"\n{'='*60}")
        print(f"{name}")
        print(f"{'='*60}")
        print(f"  Path: {path}")
        print(f"  Target: {cpp_class_name}")

        try:
            # Load
            print(f"\n  [1/4] Loading...")
            bp = unreal.load_asset(path)
            if not bp:
                print(f"  ERROR: Could not load asset")
                continue
            print(f"  OK: Loaded")

            # Get current variable count
            vars = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
            print(f"  Current variables: {len(vars)}")

            # Step 1: Clear EventGraph
            print(f"\n  [2/4] Clearing EventGraph...")
            cleared = unreal.SLFAutomationLibrary.clear_event_graphs(bp)
            print(f"  Cleared {cleared} nodes from EventGraph")

            # Step 2: Remove ALL Blueprint variables
            print(f"\n  [3/4] Removing ALL Blueprint variables...")
            removed = unreal.SLFAutomationLibrary.remove_all_variables(bp)
            print(f"  Removed {removed} variables")

            # GC before reparenting
            gc.collect()
            unreal.SystemLibrary.collect_garbage()

            # Step 3: Reparent to C++
            print(f"\n  [4/4] Reparenting to {cpp_class_name}...")
            reparented = unreal.SLFAutomationLibrary.reparent_blueprint(bp, cpp_parent)
            if reparented:
                print(f"  OK: Reparented")
            else:
                print(f"  WARNING: Reparent returned False")

            # Save
            print(f"\n  Saving...")
            saved = unreal.EditorAssetLibrary.save_asset(path)
            if saved:
                print(f"  OK: Saved")
                success_count += 1
            else:
                print(f"  WARNING: Save returned False")

        except Exception as e:
            print(f"  ERROR: {e}")
            import traceback
            traceback.print_exc()

    print("")
    print("=" * 70)
    print(f"RESULT: {success_count}/{len(ANIMBP_MIGRATION)} migrated successfully")
    print("=" * 70)

    if success_count == len(ANIMBP_MIGRATION):
        print("")
        print("All AnimBPs migrated to C++ AnimInstance classes.")
        print("")
        print("C++ NativeUpdateAnimation() provides:")
        print("  - GroundSpeed, Velocity, IsFalling, Direction")
        print("  - SoulslikeEnemy/SoulslikeCharacter references")
        print("  - MovementComponent")
        print("")
        print("AnimGraph Property Access reads these C++ properties.")


if __name__ == "__main__":
    main()
