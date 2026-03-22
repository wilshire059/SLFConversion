# fix_animbp_variables.py
# Remove Blueprint variables that now exist in C++ parent classes
#
# When AnimBPs are reparented to C++ AnimInstance classes, the Blueprint
# may still have its own variables with the same names. This causes
# "property already exists" errors. This script removes the conflicting
# Blueprint variables so the C++ properties are used instead.

import unreal

# Variables to remove from each AnimBP (they now exist in C++ parent)
VARIABLES_TO_REMOVE = {
    "ABP_SoulslikeEnemy": [
        "SoulslikeCharacter",
        "MovementComponent",
        "Velocity",
        "GroundSpeed",
        "IsFalling",
        "EquipmentComponent",
        "LocomotionType",
        "PhysicsWeight",
        "SoulslikeEnemy",
        "ACAICombatManager",
        "HitLocation",
        "Direction",
        "PoiseBroken",
        "PoiseBreakAsset",
    ],
    "ABP_SoulslikeNPC": [
        "SoulslikeCharacter",
        "MovementComponent",
        "Velocity",
        "GroundSpeed",
        "IsFalling",
        "Direction",
        "MaxDistance",
        "LookAtLocation",
        "Speed",
    ],
    "ABP_SoulslikeBossNew": [
        "SoulslikeCharacter",
        "MovementComponent",
        "Velocity",
        "GroundSpeed",
        "IsFalling",
        "PhysicsWeight",
        "Direction",
        "HitLocation",
        "PoiseBroken",
        "PoiseBreakAsset",
    ],
}

ANIMBP_PATHS = {
    "ABP_SoulslikeEnemy": "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeEnemy",
    "ABP_SoulslikeNPC": "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeNPC",
    "ABP_SoulslikeBossNew": "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeBossNew",
}


def main():
    print("=" * 70)
    print("FIX ANIMBP VARIABLES")
    print("=" * 70)
    print("")
    print("Removing Blueprint variables that conflict with C++ parent properties.")
    print("")

    total_removed = 0

    for name, path in ANIMBP_PATHS.items():
        print(f"\n--- {name} ---")
        print(f"  Path: {path}")

        try:
            bp = unreal.load_asset(path)
            if not bp:
                print(f"  ERROR: Could not load")
                continue

            vars_to_remove = VARIABLES_TO_REMOVE.get(name, [])
            if not vars_to_remove:
                print(f"  SKIP: No variables to remove")
                continue

            print(f"  Removing {len(vars_to_remove)} conflicting variables...")

            removed = unreal.SLFAutomationLibrary.remove_variables(bp, vars_to_remove)
            print(f"  Removed: {removed}")
            total_removed += removed

            # Save
            saved = unreal.EditorAssetLibrary.save_asset(path)
            if saved:
                print(f"  OK: Saved")
            else:
                print(f"  WARNING: Save returned False")

        except Exception as e:
            print(f"  ERROR: {e}")

    print("")
    print("=" * 70)
    print(f"DONE - Removed {total_removed} total variables")
    print("=" * 70)
    print("")
    print("The AnimBPs should now compile without variable conflicts.")
    print("C++ properties are used instead of Blueprint variables.")


if __name__ == "__main__":
    main()
