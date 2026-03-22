# check_container_scs.py
# Check what components B_Container's Blueprint SCS has

import unreal

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("CHECKING B_CONTAINER SCS COMPONENTS")
    unreal.log_warning("=" * 70)

    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Container"
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_error(f"Could not load: {bp_path}")
        return

    # Get the SimpleConstructionScript
    try:
        scs = bp.get_editor_property("simple_construction_script")
        if scs:
            unreal.log_warning(f"SCS found: {scs}")

            # Try to get root nodes
            try:
                root_nodes = scs.get_editor_property("root_nodes")
                unreal.log_warning(f"Root nodes: {len(root_nodes) if root_nodes else 0}")
                if root_nodes:
                    for i, node in enumerate(root_nodes):
                        comp_template = node.get_editor_property("component_template")
                        if comp_template:
                            unreal.log_warning(f"  [{i}] {comp_template.get_name()} ({type(comp_template).__name__})")
            except Exception as e:
                unreal.log_warning(f"root_nodes error: {e}")

            # Try to get all nodes
            try:
                all_nodes = scs.get_editor_property("all_nodes")
                unreal.log_warning(f"\nAll nodes: {len(all_nodes) if all_nodes else 0}")
                if all_nodes:
                    for i, node in enumerate(all_nodes):
                        comp_template = node.get_editor_property("component_template")
                        if comp_template:
                            parent_node = node.get_editor_property("parent_component_or_variable_name")
                            unreal.log_warning(f"  [{i}] {comp_template.get_name()} ({type(comp_template).__name__}) parent: {parent_node}")
            except Exception as e:
                unreal.log_warning(f"all_nodes error: {e}")
        else:
            unreal.log_warning("No SCS found")
    except Exception as e:
        unreal.log_error(f"SCS error: {e}")

    # Also check CDO components
    unreal.log_warning("\n--- CDO COMPONENTS ---")
    gen_class = bp.generated_class()
    if gen_class:
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            # Get all components on CDO
            try:
                components = cdo.get_components_by_class(unreal.ActorComponent)
                unreal.log_warning(f"CDO components: {len(components) if components else 0}")
                if components:
                    for comp in components:
                        outer = comp.get_outer()
                        unreal.log_warning(f"  - {comp.get_name()} ({type(comp).__name__})")
            except Exception as e:
                unreal.log_warning(f"CDO components error: {e}")

    unreal.log_warning("=" * 70)

if __name__ == "__main__":
    main()
