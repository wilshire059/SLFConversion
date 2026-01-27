"""
Check B_Door_Demo SCS (SimpleConstructionScript) for component definitions.
The CDO may not have components if they're defined in SCS.
"""
import unreal
import json

results = {}

bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Door_Demo"

unreal.log("=" * 70)
unreal.log("B_Door_Demo SCS CHECK")
unreal.log("=" * 70)

bp = unreal.EditorAssetLibrary.load_asset(bp_path)
if bp:
    unreal.log(f"Blueprint loaded: {bp.get_name()}")
    results["blueprint_name"] = bp.get_name()

    # Try to get the SimpleConstructionScript
    try:
        scs = bp.get_editor_property('simple_construction_script')
        if scs:
            unreal.log(f"SCS found: {scs}")
            results["has_scs"] = True

            # Try to get nodes
            try:
                nodes = scs.get_editor_property('all_nodes')
                if nodes:
                    unreal.log(f"SCS has {len(nodes)} nodes")
                    results["scs_node_count"] = len(nodes)
                    results["scs_nodes"] = []
                    for node in nodes:
                        node_info = {"name": node.get_name()}
                        unreal.log(f"  Node: {node.get_name()}")
                        # Try to get component template
                        try:
                            template = node.get_editor_property('component_template')
                            if template:
                                node_info["template_name"] = template.get_name()
                                node_info["template_class"] = template.get_class().get_name()
                                unreal.log(f"    Template: {template.get_name()} ({template.get_class().get_name()})")
                                if isinstance(template, unreal.StaticMeshComponent):
                                    mesh = template.static_mesh
                                    node_info["mesh"] = mesh.get_name() if mesh else "NO_MESH"
                                    unreal.log(f"    Mesh: {node_info['mesh']}")
                        except Exception as e:
                            unreal.log(f"    Template error: {e}")
                        results["scs_nodes"].append(node_info)
            except Exception as e:
                unreal.log(f"Nodes error: {e}")
        else:
            unreal.log("No SCS found")
            results["has_scs"] = False
    except Exception as e:
        unreal.log(f"SCS error: {e}")

    # Also try to get the default object another way - using spawning
    unreal.log("\n" + "=" * 70)
    unreal.log("SPAWN TEST")
    unreal.log("=" * 70)

    gen_class = bp.generated_class()
    if gen_class:
        # Spawn the actor to see what components it actually has
        world = unreal.EditorLevelLibrary.get_editor_world()
        if world:
            location = unreal.Vector(99999, 99999, 0)
            rotation = unreal.Rotator(0, 0, 0)
            spawned = unreal.EditorLevelLibrary.spawn_actor_from_class(gen_class, location, rotation)
            if spawned:
                unreal.log(f"Spawned: {spawned.get_name()}")
                results["spawned_components"] = []

                # Check all components on spawned actor
                all_comps = spawned.get_components_by_class(unreal.ActorComponent)
                unreal.log(f"Spawned actor has {len(all_comps)} components")
                for comp in all_comps:
                    comp_info = {
                        "name": comp.get_name(),
                        "class": comp.get_class().get_name()
                    }
                    unreal.log(f"  [{comp.get_name()}] {comp.get_class().get_name()}")
                    if isinstance(comp, unreal.StaticMeshComponent):
                        mesh = comp.static_mesh
                        comp_info["mesh"] = mesh.get_name() if mesh else "NO_MESH"
                        comp_info["visible"] = comp.is_visible()
                        unreal.log(f"      Mesh: {comp_info['mesh']}, Visible: {comp_info['visible']}")
                    results["spawned_components"].append(comp_info)

                # Clean up
                spawned.destroy_actor()

# Save
output_path = "C:/scripts/SLFConversion/migration_cache/door_demo_scs.json"
with open(output_path, "w") as f:
    json.dump(results, f, indent=2)
unreal.log(f"\nSaved to {output_path}")
