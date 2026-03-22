# check_bp_parent_class.py
# Check what parent class B_RestingPoint actually has

import unreal

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("CHECKING B_RestingPoint PARENT CLASS")
    unreal.log_warning("=" * 70)

    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_RestingPoint"
    bp = unreal.load_asset(bp_path)

    if not bp:
        unreal.log_error(f"Could not load Blueprint: {bp_path}")
        return

    # Get generated class
    gen_class = bp.generated_class()
    if gen_class:
        unreal.log_warning(f"Generated class: {gen_class.get_name()}")
        unreal.log_warning(f"Generated class path: {gen_class.get_path_name()}")

        # Get parent class
        parent_class = gen_class.get_super_class()
        if parent_class:
            unreal.log_warning(f"PARENT CLASS: {parent_class.get_name()}")
            unreal.log_warning(f"PARENT CLASS PATH: {parent_class.get_path_name()}")

            # Walk up the hierarchy
            unreal.log_warning("\nClass hierarchy:")
            current = gen_class
            depth = 0
            while current:
                prefix = "  " * depth
                unreal.log_warning(f"{prefix}- {current.get_name()} ({current.get_path_name()})")
                current = current.get_super_class()
                depth += 1
                if depth > 10:
                    break

    # Check if it implements the interface
    unreal.log_warning("\nInterface check:")
    try:
        interface_class = unreal.load_class(None, "/Script/SLFConversion.SLFInteractableInterface")
        if interface_class:
            unreal.log_warning(f"Interface class loaded: {interface_class.get_name()}")
        else:
            unreal.log_warning("Could not load SLFInteractableInterface class")
    except Exception as e:
        unreal.log_error(f"Error loading interface: {e}")

    # Also check B_Container for comparison
    unreal.log_warning("\n" + "=" * 70)
    unreal.log_warning("CHECKING B_Container PARENT CLASS (for comparison)")
    unreal.log_warning("=" * 70)

    bp_path2 = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Container"
    bp2 = unreal.load_asset(bp_path2)

    if bp2:
        gen_class2 = bp2.generated_class()
        if gen_class2:
            parent2 = gen_class2.get_super_class()
            unreal.log_warning(f"B_Container generated class: {gen_class2.get_name()}")
            unreal.log_warning(f"B_Container PARENT CLASS: {parent2.get_name() if parent2 else 'None'}")
            unreal.log_warning(f"B_Container PARENT PATH: {parent2.get_path_name() if parent2 else 'None'}")

            # Walk up the hierarchy
            unreal.log_warning("\nClass hierarchy:")
            current = gen_class2
            depth = 0
            while current:
                prefix = "  " * depth
                unreal.log_warning(f"{prefix}- {current.get_name()} ({current.get_path_name()})")
                current = current.get_super_class()
                depth += 1
                if depth > 10:
                    break

if __name__ == "__main__":
    main()
