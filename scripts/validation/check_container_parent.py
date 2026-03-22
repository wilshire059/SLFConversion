# check_container_parent.py
# Check the parent class hierarchy of B_Container and B_Interactable

import unreal

def walk_bp_hierarchy(bp_name, bp_path):
    """Walk Blueprint hierarchy using generated_class and CDO"""
    unreal.log_warning(f"\n{bp_name}:")

    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_error(f"  Could not load: {bp_path}")
        return

    # Get the generated class
    gen_class = bp.generated_class()
    if not gen_class:
        unreal.log_error(f"  No generated class!")
        return

    unreal.log_warning(f"  Generated: {gen_class.get_name()}")

    # Get the CDO and check its class hierarchy
    cdo = unreal.get_default_object(gen_class)
    if cdo:
        unreal.log_warning(f"  CDO Type: {type(cdo).__name__}")
        unreal.log_warning(f"  CDO Class: {cdo.get_class().get_name()}")

        # Get the class directly from CDO
        obj_class = cdo.get_class()
        unreal.log_warning(f"  Full Path: {obj_class.get_path_name()}")

    # Use export_text to see the raw Blueprint data
    try:
        export = bp.export_text()
        # Look for ParentClass line
        for line in export.split('\n')[:100]:
            if 'ParentClass' in line:
                unreal.log_warning(f"  {line.strip()}")
                break
    except Exception as e:
        unreal.log_error(f"  export_text error: {e}")

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("CHECKING CONTAINER PARENT HIERARCHY")
    unreal.log_warning("=" * 70)

    blueprints = [
        ("B_Container", "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Container"),
        ("B_Interactable", "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Interactable"),
        ("B_RestingPoint", "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_RestingPoint"),
    ]

    for bp_name, bp_path in blueprints:
        walk_bp_hierarchy(bp_name, bp_path)

    unreal.log_warning("=" * 70)

if __name__ == "__main__":
    main()
