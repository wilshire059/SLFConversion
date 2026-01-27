# fix_animbps_nocompile.py
# Fix AnimBPs using NoCompile version to avoid crash during broken compilation
# Then reparent to C++ parent class

import unreal

# Map AnimBP paths to their C++ parent classes
ANIMBP_MAP = {
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeEnemy": "/Script/SLFConversion.ABP_SoulslikeEnemy",
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeNPC": "/Script/SLFConversion.ABP_SoulslikeNPC",
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeBossNew": "/Script/SLFConversion.ABP_SoulslikeBossNew",
}


def main():
    print("=" * 60)
    print("FIX ANIMBPS (NoCompile + Reparent)")
    print("=" * 60)

    success_count = 0
    fail_count = 0

    for bp_path, cpp_parent in ANIMBP_MAP.items():
        name = bp_path.split("/")[-1]
        print(f"\n{name}:")
        print(f"  Target Parent: {cpp_parent.split('.')[-1]}")

        # Load the AnimBP
        try:
            bp = unreal.load_asset(bp_path)
        except Exception as e:
            print(f"  FAILED TO LOAD: {e}")
            fail_count += 1
            continue

        if not bp:
            print("  FAILED: Could not load AnimBP")
            fail_count += 1
            continue

        # Step 1: Clear EventGraph using NoCompile version (avoids crash)
        print("  Step 1: Clearing EventGraph (NoCompile)...")
        try:
            cleared = unreal.SLFAutomationLibrary.clear_graphs_keep_variables_no_compile(bp)
            if cleared:
                print("    Cleared EventGraph")
            else:
                print("    Warning: Clear returned False")
        except Exception as e:
            print(f"    Clear failed: {e}")

        # Step 2: Reparent to C++ class
        print("  Step 2: Reparenting...")
        try:
            reparented = unreal.SLFAutomationLibrary.reparent_blueprint(bp, cpp_parent)
            if reparented:
                print("    Reparented successfully")
            else:
                print("    Warning: Reparent returned False")
        except Exception as e:
            print(f"    Reparent failed: {e}")

        # Step 3: Save asset
        print("  Step 3: Saving...")
        try:
            saved = unreal.EditorAssetLibrary.save_asset(bp_path)
            if saved:
                print("    Saved successfully")
                success_count += 1
            else:
                print("    Save failed")
                fail_count += 1
        except Exception as e:
            print(f"    Save exception: {e}")
            fail_count += 1

    print("\n" + "=" * 60)
    print(f"RESULT: {success_count} succeeded, {fail_count} failed")
    print("=" * 60)


if __name__ == "__main__":
    main()
