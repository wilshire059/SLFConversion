# fix_animbp_eventgraphs.py
# Clear EventGraph from AnimBPs while keeping variables
# This fixes compilation errors from missing properties like "Poise Break Asset"
# The C++ AnimInstance (UABP_SoulslikeEnemy) handles variable updates

import unreal

ANIMBP_PATHS = [
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeEnemy",
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeNPC",
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeBossNew",
]

def main():
    print("=" * 60)
    print("CLEARING ANIMBP EVENTGRAPHS (Keep Variables for AnimGraph)")
    print("=" * 60)

    success_count = 0
    fail_count = 0

    for bp_path in ANIMBP_PATHS:
        name = bp_path.split("/")[-1]
        print(f"\n{name}:")

        # Load the asset
        bp = unreal.load_asset(bp_path)
        if not bp:
            print(f"  FAILED: Could not load")
            fail_count += 1
            continue

        # Clear graphs but keep variables
        # This uses the SLFAutomationLibrary C++ function
        cleared = unreal.SLFAutomationLibrary.clear_graphs_keep_variables(bp)

        if cleared:
            # Save the asset
            saved = unreal.EditorAssetLibrary.save_asset(bp_path)
            if saved:
                print(f"  OK: Cleared EventGraph, saved")
                success_count += 1
            else:
                print(f"  PARTIAL: Cleared but save failed")
                fail_count += 1
        else:
            print(f"  FAILED: Could not clear graphs")
            fail_count += 1

    print("\n" + "=" * 60)
    print(f"RESULT: {success_count} succeeded, {fail_count} failed")
    print("=" * 60)
    print()
    print("Next step: The C++ AnimInstance classes will update animation")
    print("variables via NativeUpdateAnimation(). The AnimGraph reads")
    print("these variables to drive animations.")


if __name__ == "__main__":
    main()
