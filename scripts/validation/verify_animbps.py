# verify_animbps.py
# Verify AnimBP migration status

import unreal

ANIMBP_PATHS = [
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeEnemy",
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeNPC",
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeBossNew",
]


def main():
    print("=" * 70)
    print("VERIFY ANIMBP MIGRATION")
    print("=" * 70)

    for path in ANIMBP_PATHS:
        name = path.split("/")[-1]
        print(f"\n{name}:")

        try:
            bp = unreal.load_asset(path)
            if not bp:
                print(f"  ERROR: Could not load")
                continue

            # Check parent class
            parent_str = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
            print(f"  Parent: {parent_str}")

            # Check variables
            vars = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
            print(f"  Variables: {len(vars)}")
            if len(vars) > 0:
                for v in vars[:5]:
                    print(f"    - {v}")
                if len(vars) > 5:
                    print(f"    ... and {len(vars) - 5} more")

            # Check compile status
            errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
            if errors and len(errors) > 0:
                print(f"  Compile errors: Yes")
                for line in errors.split("\n")[:3]:
                    if line.strip():
                        print(f"    {line.strip()[:80]}")
            else:
                print(f"  Compile errors: None")

        except Exception as e:
            print(f"  ERROR: {e}")

    print("\n" + "=" * 70)
    print("DONE")
    print("=" * 70)


if __name__ == "__main__":
    main()
