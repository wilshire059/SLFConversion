# fix_animbp_defaults.py
# Set default values for AnimBP variables that are causing None access errors
# This fixes the "Accessed None trying to read PoiseBreakAsset" error

import unreal

ANIMBP_PATHS = [
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeEnemy",
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeNPC",
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeBossNew",
]

# PDA_PoiseBreakAnimData path
POISE_BREAK_ASSET_PATH = "/Game/SoulslikeFramework/Data/_AnimationData/PDA_PoiseBreakAnimData"


def main():
    print("=" * 60)
    print("FIX ANIMBP DEFAULT VALUES")
    print("=" * 60)

    # Load the PoiseBreakAsset
    poise_break_asset = unreal.load_asset(POISE_BREAK_ASSET_PATH)
    if not poise_break_asset:
        print(f"FAILED: Could not load {POISE_BREAK_ASSET_PATH}")
        return

    print(f"Loaded PoiseBreakAsset: {poise_break_asset.get_name()}")

    success_count = 0
    fail_count = 0

    for bp_path in ANIMBP_PATHS:
        name = bp_path.split("/")[-1]
        print(f"\n{name}:")

        # Load the AnimBP
        bp = unreal.load_asset(bp_path)
        if not bp:
            print("  FAILED: Could not load")
            fail_count += 1
            continue

        # Get the generated class
        gen_class = bp.get_editor_property("generated_class")
        if not gen_class:
            print("  FAILED: No generated class")
            fail_count += 1
            continue

        # Get the CDO (Class Default Object)
        cdo = unreal.get_default_object(gen_class)
        if not cdo:
            print("  FAILED: Could not get CDO")
            fail_count += 1
            continue

        # Set the PoiseBreakAsset default value
        try:
            cdo.set_editor_property("PoiseBreakAsset", poise_break_asset)
            print(f"  Set PoiseBreakAsset = {poise_break_asset.get_name()}")
        except Exception as e:
            print(f"  WARNING: Could not set PoiseBreakAsset: {e}")

        # Save the asset
        try:
            saved = unreal.EditorAssetLibrary.save_asset(bp_path)
            if saved:
                print(f"  OK: Saved")
                success_count += 1
            else:
                print(f"  WARNING: Save returned False")
                fail_count += 1
        except Exception as e:
            print(f"  ERROR saving: {e}")
            fail_count += 1

    print("\n" + "=" * 60)
    print(f"RESULT: {success_count} succeeded, {fail_count} failed")
    print("=" * 60)


if __name__ == "__main__":
    main()
