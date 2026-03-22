# check_animbp_parents.py
# Check if AnimBPs are using C++ parent classes

import unreal

ANIMBP_PATHS = [
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeEnemy",
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeNPC",
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeBossNew",
]

def main():
    print("=" * 60)
    print("ANIMBP PARENT CLASS CHECK")
    print("=" * 60)

    for path in ANIMBP_PATHS:
        name = path.split("/")[-1]
        print(f"\n{name}:")

        bp = unreal.load_asset(path)
        if not bp:
            print("  FAILED TO LOAD")
            continue

        # Get the generated class
        gen_class = bp.get_editor_property("generated_class")
        if not gen_class:
            print("  No generated class")
            continue

        # Get parent class
        parent = gen_class.get_super_class()
        if parent:
            parent_name = parent.get_name()
            parent_path = parent.get_path_name()
            print(f"  Parent Class: {parent_name}")
            print(f"  Parent Path: {parent_path}")

            # Check if using C++ class
            if "/Script/SLFConversion" in parent_path:
                print("  STATUS: Using C++ parent (GOOD)")
            elif "AnimInstance" in parent_name:
                print("  STATUS: Using base AnimInstance - NEEDS REPARENTING")
            else:
                print(f"  STATUS: Unknown parent type")
        else:
            print("  No parent found")

    print("\n" + "=" * 60)


if __name__ == "__main__":
    main()
