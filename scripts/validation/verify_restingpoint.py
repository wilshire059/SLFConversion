# verify_restingpoint.py
# Verify B_RestingPoint interface implementation

import unreal

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("VERIFYING B_RestingPoint")
    unreal.log_warning("=" * 70)

    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_RestingPoint"

    # Load the Blueprint
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_error(f"Could not load Blueprint: {bp_path}")
        return

    unreal.log_warning(f"Blueprint loaded: {bp.get_name()}")

    # Get generated class and CDO
    gen_class = bp.generated_class()
    if not gen_class:
        unreal.log_error("Could not get generated class")
        return

    unreal.log_warning(f"Generated class: {gen_class.get_name()}")

    cdo = unreal.get_default_object(gen_class)
    if cdo:
        unreal.log_warning(f"CDO class: {cdo.get_class().get_name()}")
        unreal.log_warning(f"CDO path: {cdo.get_class().get_path_name()}")

    # Check interface implementation
    interface_class = unreal.load_class(None, "/Script/SLFConversion.SLFInteractableInterface")
    if interface_class:
        implements = gen_class.implements_interface(interface_class)
        unreal.log_warning(f"Implements ISLFInteractableInterface: {implements}")
    else:
        unreal.log_warning("Could not load interface class")

    # Check parent class
    try:
        # Get parent by checking the CDO's outer
        parent = gen_class.get_super_class()
        if parent:
            unreal.log_warning(f"Parent class: {parent.get_name()}")
            unreal.log_warning(f"Parent path: {parent.get_path_name()}")
    except Exception as e:
        unreal.log_warning(f"Error getting parent: {e}")

    unreal.log_warning("=" * 70)

if __name__ == "__main__":
    main()
