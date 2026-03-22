# check_parent_simple.py
# Check what parent classes the Blueprints have

import unreal

BLUEPRINTS_TO_CHECK = [
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Interactable",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Container",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_RestingPoint",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/LevelDesign/B_Destructible",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/B_Chaos_ForceField",
]

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("PARENT CLASS CHECK")
    unreal.log_warning("=" * 70)

    for bp_path in BLUEPRINTS_TO_CHECK:
        bp_name = bp_path.split('/')[-1]
        bp = unreal.load_asset(bp_path)
        if not bp:
            unreal.log_error(f"{bp_name}: Could not load")
            continue

        # Get generated class
        gen_class = bp.generated_class()
        if not gen_class:
            unreal.log_error(f"{bp_name}: No generated class")
            continue

        unreal.log_warning(f"\n{bp_name}:")
        unreal.log_warning(f"  Generated class: {gen_class.get_name()}")
        unreal.log_warning(f"  Generated path: {gen_class.get_path_name()}")

        # Get CDO and check components
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            unreal.log_warning(f"  CDO class: {cdo.get_class().get_name()}")
            unreal.log_warning(f"  CDO path: {cdo.get_class().get_path_name()}")

            components = cdo.get_components_by_class(unreal.ActorComponent)
            unreal.log_warning(f"  CDO components count: {len(components)}")
            for c in components:
                unreal.log_warning(f"    - {c.get_name()} ({c.get_class().get_name()})")

            # Check root
            try:
                root = cdo.get_editor_property('root_component')
                if root:
                    unreal.log_warning(f"  Root: {root.get_name()} ({root.get_class().get_name()})")
                else:
                    unreal.log_warning(f"  Root: None")
            except Exception as e:
                unreal.log_warning(f"  Root: Error - {e}")

        # Try to get outer chain to find parent
        outer = gen_class.get_outer()
        if outer:
            unreal.log_warning(f"  Outer: {outer.get_name()}")

if __name__ == "__main__":
    main()
