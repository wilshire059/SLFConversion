# reparent_animbps.py
# Reparent AnimBPs to C++ AnimInstance classes
# This fixes compilation errors from Blueprint-typed variables

import unreal

# Map AnimBP paths to their C++ parent classes
ANIMBP_REPARENT_MAP = {
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeEnemy": "/Script/SLFConversion.ABP_SoulslikeEnemy",
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeNPC": "/Script/SLFConversion.ABP_SoulslikeNPC",
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeBossNew": "/Script/SLFConversion.ABP_SoulslikeBossNew",
}


def main():
    print("=" * 60)
    print("REPARENTING ANIMBPS TO C++ ANIMINSTANCE CLASSES")
    print("=" * 60)

    success_count = 0
    fail_count = 0

    for bp_path, cpp_parent in ANIMBP_REPARENT_MAP.items():
        name = bp_path.split("/")[-1]
        print(f"\n{name}:")
        print(f"  Target Parent: {cpp_parent.split('.')[-1]}")

        # Load the AnimBP
        bp = unreal.load_asset(bp_path)
        if not bp:
            print("  FAILED: Could not load AnimBP")
            fail_count += 1
            continue

        # Load the C++ parent class
        parent_class = unreal.load_class(None, cpp_parent)
        if not parent_class:
            print(f"  FAILED: Could not load parent class {cpp_parent}")
            fail_count += 1
            continue

        # Reparent using SLFAutomationLibrary
        success = unreal.SLFAutomationLibrary.reparent_blueprint(bp, cpp_parent)

        if success:
            # Clear EventGraph (will be handled by C++ NativeUpdateAnimation)
            cleared = unreal.SLFAutomationLibrary.clear_graphs_keep_variables(bp)

            # Save
            saved = unreal.EditorAssetLibrary.save_asset(bp_path)
            if saved:
                print(f"  OK: Reparented and saved")
                success_count += 1
            else:
                print(f"  PARTIAL: Reparented but save failed")
                fail_count += 1
        else:
            print(f"  FAILED: Reparent failed")
            fail_count += 1

    print("\n" + "=" * 60)
    print(f"RESULT: {success_count} succeeded, {fail_count} failed")
    print("=" * 60)
    print()
    print("After reparenting:")
    print("- AnimGraph reads variables from C++ UPROPERTY declarations")
    print("- C++ NativeUpdateAnimation() updates these variables each frame")
    print("- Blueprint EventGraph logic is no longer needed")


if __name__ == "__main__":
    main()
