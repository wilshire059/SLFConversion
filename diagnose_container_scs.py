# diagnose_container_scs.py
# Diagnose B_Container SCS components and hierarchy

import unreal

BP_CONTAINER_PATH = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Container"

def diagnose_container():
    """Diagnose B_Container SCS and component hierarchy"""
    unreal.log_warning("=" * 60)
    unreal.log_warning("DIAGNOSING B_CONTAINER SCS")
    unreal.log_warning("=" * 60)

    # Load the Blueprint
    bp_asset = unreal.load_asset(BP_CONTAINER_PATH)
    if not bp_asset:
        unreal.log_error(f"Could not load Blueprint: {BP_CONTAINER_PATH}")
        return

    unreal.log_warning(f"Blueprint: {bp_asset.get_name()}")

    # Get SCS
    try:
        scs = bp_asset.get_editor_property("simple_construction_script")
        if scs:
            unreal.log_warning(f"SCS: {scs}")

            # Get all nodes
            try:
                all_nodes = scs.get_all_nodes()
                unreal.log_warning(f"\n--- SCS NODES ({len(all_nodes)}) ---")
                for node in all_nodes:
                    try:
                        comp_template = node.component_template
                        if comp_template:
                            parent_node = node.get_editor_property("parent_component_or_variable_name")
                            is_root = node.get_editor_property("is_root_node") if hasattr(node, 'is_root_node') else "?"
                            unreal.log_warning(f"  {comp_template.get_name()} ({type(comp_template).__name__}) - parent: {parent_node}")
                    except Exception as e:
                        unreal.log_warning(f"  Node error: {e}")
            except Exception as e:
                unreal.log_warning(f"get_all_nodes error: {e}")
        else:
            unreal.log_warning("No SCS found!")
    except Exception as e:
        unreal.log_warning(f"SCS error: {e}")

    # Spawn actor and check component hierarchy
    unreal.log_warning("\n--- SPAWNED ACTOR COMPONENTS ---")
    gen_class = bp_asset.generated_class()
    world = unreal.EditorLevelLibrary.get_editor_world()
    if world:
        actor = unreal.EditorLevelLibrary.spawn_actor_from_class(gen_class, unreal.Vector(0, 0, 0))
        if actor:
            # Get root component
            root = actor.get_editor_property("root_component")
            unreal.log_warning(f"Root Component: {root.get_name() if root else 'None'} ({type(root).__name__ if root else 'N/A'})")
            if root:
                root_loc = root.get_editor_property("relative_location")
                unreal.log_warning(f"  Root Location: {root_loc}")

            # Get all components
            all_comps = actor.get_components_by_class(unreal.ActorComponent)
            unreal.log_warning(f"\nAll Components ({len(all_comps)}):")
            for comp in all_comps:
                try:
                    comp_name = comp.get_name()
                    comp_type = type(comp).__name__

                    # Check if scene component and get transform
                    if isinstance(comp, unreal.SceneComponent):
                        loc = comp.get_editor_property("relative_location")
                        attach_parent = comp.get_editor_property("attach_parent")
                        parent_name = attach_parent.get_name() if attach_parent else "None"
                        unreal.log_warning(f"  {comp_name} ({comp_type}) - parent: {parent_name}, loc: {loc}")

                        # Check for mesh
                        if isinstance(comp, unreal.StaticMeshComponent):
                            mesh = comp.get_editor_property("static_mesh")
                            unreal.log_warning(f"    Mesh: {mesh.get_name() if mesh else 'None'}")
                    else:
                        unreal.log_warning(f"  {comp_name} ({comp_type})")
                except Exception as e:
                    unreal.log_warning(f"  Component error: {e}")

            # Check collision
            unreal.log_warning("\n--- COLLISION CHECK ---")
            try:
                # Look for collision component
                for comp in all_comps:
                    if "collision" in comp.get_name().lower() or isinstance(comp, unreal.BoxComponent) or isinstance(comp, unreal.SphereComponent) or isinstance(comp, unreal.CapsuleComponent):
                        unreal.log_warning(f"  Found collision: {comp.get_name()} ({type(comp).__name__})")
                        if hasattr(comp, 'box_extent'):
                            extent = comp.get_editor_property("box_extent")
                            unreal.log_warning(f"    Extent: {extent}")
            except Exception as e:
                unreal.log_warning(f"  Collision check error: {e}")

            actor.destroy_actor()

    unreal.log_warning("\n" + "=" * 60)
    unreal.log_warning("DIAGNOSIS COMPLETE")
    unreal.log_warning("=" * 60)

if __name__ == "__main__":
    diagnose_container()
