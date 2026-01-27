# check_stat_hierarchy.py
# Verify the class hierarchy of stat Blueprints

import unreal

def check_stat_hierarchy():
    """Check the parent classes of stat Blueprints."""

    stat_blueprints = [
        "/Game/SoulslikeFramework/Data/Stats/Primary/B_Vigor.B_Vigor_C",
        "/Game/SoulslikeFramework/Data/Stats/Primary/B_Mind.B_Mind_C",
        "/Game/SoulslikeFramework/Data/Stats/Primary/B_Strength.B_Strength_C",
    ]

    unreal.log_warning("=" * 60)
    unreal.log_warning("Checking stat Blueprint class hierarchy")
    unreal.log_warning("=" * 60)

    for bp_path in stat_blueprints:
        stat_class = unreal.load_class(None, bp_path)
        if not stat_class:
            unreal.log_warning(f"Could not load: {bp_path}")
            continue

        class_name = stat_class.get_name()
        parent_class = stat_class.get_super_class()

        unreal.log_warning(f"\nBlueprint: {class_name}")
        unreal.log_warning(f"  Parent: {parent_class.get_name() if parent_class else 'None'}")

        # Walk up the hierarchy
        current = parent_class
        depth = 1
        while current:
            grandparent = current.get_super_class()
            unreal.log_warning(f"  {'  ' * depth}-> {grandparent.get_name() if grandparent else 'None'}")
            current = grandparent
            depth += 1
            if depth > 5:
                break

    # Also check the C++ stat classes
    unreal.log_warning("\n" + "=" * 60)
    unreal.log_warning("Checking C++ stat class hierarchy")
    unreal.log_warning("=" * 60)

    cpp_classes = [
        "/Script/SLFConversion.SLFStatVigor",
        "/Script/SLFConversion.SLFStatMind",
        "/Script/SLFConversion.SLFStatStrength",
    ]

    for cpp_path in cpp_classes:
        cpp_class = unreal.load_class(None, cpp_path)
        if not cpp_class:
            unreal.log_warning(f"Could not load: {cpp_path}")
            continue

        class_name = cpp_class.get_name()
        parent_class = cpp_class.get_super_class()

        unreal.log_warning(f"\nC++ Class: {class_name}")
        unreal.log_warning(f"  Parent: {parent_class.get_name() if parent_class else 'None'}")

    unreal.log_warning("=" * 60)

if __name__ == "__main__":
    check_stat_hierarchy()
