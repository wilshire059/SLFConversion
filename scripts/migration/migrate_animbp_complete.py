# migrate_animbp_complete.py
# Complete AnimBP Migration: Clear ALL Blueprint logic and variables
#
# The issue: When reparenting AnimBPs to C++, Blueprint variables conflict
# with C++ properties of the same name, causing compilation errors.
#
# Solution: Clear ALL Blueprint logic INCLUDING variables before reparenting.
# The AnimGraph Property Access nodes will then find the C++ properties by name.
#
# This is the AAA approach - no Blueprint variables, only C++ properties.

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
    print("COMPLETE ANIMBP MIGRATION")
    print("=" * 70)
    print("")
    print("Clearing ALL Blueprint logic (including variables) and reparenting.")
    print("AnimGraph Property Access nodes will use C++ properties.")
    print("")

    success_count = 0

    for name, info in ANIMBP_MIGRATION.items():
        path = info["path"]
        cpp_parent = info["cpp_parent"]

        print(f"\n--- {name} ---")
        print(f"  Path: {path}")
        print(f"  Target: {cpp_parent.split('.')[-1]}")

        try:
            bp = unreal.load_asset(path)
            if not bp:
                print(f"  ERROR: Could not load")
                continue

            print(f"  Loaded OK")

            # Step 1: Clear ALL Blueprint logic (EventGraph + Variables)
            # Use NoCompile to prevent triggering broken compilation
            print(f"  Clearing ALL Blueprint logic (including variables)...")
            cleared = unreal.SLFAutomationLibrary.clear_all_blueprint_logic_no_compile(bp)
            if cleared:
                print(f"  OK: All logic cleared")
            else:
                print(f"  WARNING: Clear returned False")

            # Force GC
            gc.collect()
            unreal.SystemLibrary.collect_garbage()

            # Step 2: Reparent to C++
            print(f"  Reparenting to {cpp_parent.split('.')[-1]}...")
            reparented = unreal.SLFAutomationLibrary.reparent_blueprint(bp, cpp_parent)
            if reparented:
                print(f"  OK: Reparented")
            else:
                print(f"  WARNING: Reparent returned False")

            # Step 3: Save
            print(f"  Saving...")
            saved = unreal.EditorAssetLibrary.save_asset(path)
            if saved:
                print(f"  OK: Saved")
                success_count += 1
            else:
                print(f"  WARNING: Save returned False")

        except Exception as e:
            print(f"  ERROR: {e}")

    print("")
    print("=" * 70)
    print(f"RESULT: {success_count}/{len(ANIMBP_MIGRATION)} migrated")
    print("=" * 70)
    print("")

    if success_count == len(ANIMBP_MIGRATION):
        print("All AnimBPs migrated successfully.")
        print("")
        print("C++ NativeUpdateAnimation() now provides:")
        print("  - GroundSpeed, Velocity, IsFalling, Direction")
        print("  - SoulslikeEnemy/SoulslikeCharacter/SoulslikeNpc references")
        print("  - MovementComponent, EquipmentComponent")
        print("")
        print("AnimGraph Property Access nodes will find these C++ properties.")


if __name__ == "__main__":
    main()
