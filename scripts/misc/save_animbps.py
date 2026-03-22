# save_animbps.py
# Save AnimBP assets after migration

import unreal

ANIMBP_PATHS = [
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeEnemy",
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeNPC",
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeBossNew",
]


def main():
    print("=" * 60)
    print("SAVING ANIMBP ASSETS")
    print("=" * 60)

    for path in ANIMBP_PATHS:
        name = path.split("/")[-1]
        print(f"\n{name}:")

        try:
            bp = unreal.load_asset(path)
            if bp:
                # Check parent class
                gen_class = bp.generated_class()
                if gen_class:
                    parent = gen_class.get_super_class()
                    if parent:
                        print(f"  Parent: {parent.get_name()}")

                # Save
                saved = unreal.EditorAssetLibrary.save_asset(path)
                if saved:
                    print(f"  OK: Saved")
                else:
                    print(f"  WARNING: Save returned False")
            else:
                print(f"  FAILED: Could not load")
        except Exception as e:
            print(f"  ERROR: {e}")

    print("\n" + "=" * 60)
    print("DONE")
    print("=" * 60)


if __name__ == "__main__":
    main()
