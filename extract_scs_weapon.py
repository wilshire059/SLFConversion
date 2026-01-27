"""
Extract Weapon ChildActorComponent configuration from Blueprint SCS.
Uses Blueprint direct access instead of CDO.
"""
import unreal
import json

ENEMY_BLUEPRINTS = [
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Guard",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Showcase",
]

def main():
    # Determine project
    project_name = "unknown"
    try:
        project_path = unreal.Paths.get_project_file_path()
        if "bp_only" in project_path.lower():
            project_name = "bp_only"
        elif "slfconversion" in project_path.lower():
            project_name = "SLFConversion"
    except:
        pass

    print(f"\n{'='*70}")
    print(f"SCS WEAPON EXTRACTION - {project_name}")
    print(f"{'='*70}")

    all_results = {}

    for bp_path in ENEMY_BLUEPRINTS:
        bp_name = bp_path.split("/")[-1]
        print(f"\n--- {bp_name} ---")

        bp = unreal.load_asset(bp_path)
        if not bp:
            print(f"  ERROR: Could not load")
            all_results[bp_name] = {"error": "Could not load"}
            continue

        result = {
            "path": bp_path,
            "scs_nodes": []
        }

        # Try different ways to access SCS
        print(f"  Blueprint type: {type(bp)}")
        print(f"  Blueprint class: {bp.get_class().get_name()}")

        # Method 1: Try Blueprint methods
        try:
            # List all methods/attributes on the Blueprint object
            bp_methods = [m for m in dir(bp) if not m.startswith('_')]
            print(f"  Available methods/properties ({len(bp_methods)}): {bp_methods[:20]}...")
        except Exception as e:
            print(f"  Error listing methods: {e}")

        # Method 2: Try to get SCS via property
        for prop_name in ['simple_construction_script', 'SimpleConstructionScript']:
            try:
                scs = bp.get_editor_property(prop_name)
                if scs:
                    print(f"  Found SCS via '{prop_name}': {scs}")
                    # Try to get nodes
                    for node_method in ['get_all_nodes', 'GetAllNodes', 'all_nodes']:
                        try:
                            if hasattr(scs, node_method):
                                nodes = getattr(scs, node_method)()
                                print(f"    Got {len(nodes)} nodes via {node_method}")
                                for node in nodes:
                                    node_info = extract_scs_node(node)
                                    result["scs_nodes"].append(node_info)
                                    print(f"      - {node_info.get('name', 'Unknown')}: {node_info}")
                        except Exception as e:
                            pass
            except Exception as e:
                print(f"  '{prop_name}' failed: {e}")

        # Method 3: Try using BlueprintEditorLibrary
        try:
            # Get all components defined in the Blueprint
            components = unreal.BlueprintEditorLibrary.get_blueprint_components(bp)
            print(f"  BlueprintEditorLibrary.get_blueprint_components: {len(components) if components else 0}")
            if components:
                for comp in components:
                    print(f"    - {comp.get_name()}: {comp.get_class().get_name()}")
                    if isinstance(comp, unreal.ChildActorComponent):
                        try:
                            child_class = comp.get_editor_property("child_actor_class")
                            if child_class:
                                print(f"        ChildActorClass: {child_class.get_name()}")
                                result["scs_nodes"].append({
                                    "name": comp.get_name(),
                                    "class": "ChildActorComponent",
                                    "child_actor_class": child_class.get_name(),
                                    "child_actor_class_path": child_class.get_path_name()
                                })
                        except Exception as e:
                            print(f"        Error getting ChildActorClass: {e}")
        except Exception as e:
            print(f"  BlueprintEditorLibrary.get_blueprint_components failed: {e}")

        # Method 4: Try spawning a temporary actor
        try:
            print(f"  Attempting to spawn temporary actor...")
            world = unreal.EditorLevelLibrary.get_editor_world()
            if world:
                gen_class = bp.generated_class()
                if gen_class:
                    # Spawn actor
                    spawn_loc = unreal.Vector(0, 0, -100000)  # Far below
                    spawn_rot = unreal.Rotator(0, 0, 0)
                    actor = unreal.EditorLevelLibrary.spawn_actor_from_class(gen_class, spawn_loc, spawn_rot)
                    if actor:
                        print(f"    Spawned actor: {actor.get_name()}")
                        # Now get ALL components including SCS ones
                        all_comps = actor.get_components_by_class(unreal.ActorComponent)
                        print(f"    Total components on spawned actor: {len(all_comps)}")

                        # Find ChildActorComponents
                        for comp in all_comps:
                            if isinstance(comp, unreal.ChildActorComponent):
                                comp_name = comp.get_name()
                                print(f"    Found ChildActorComponent: {comp_name}")
                                node_info = {
                                    "name": comp_name,
                                    "class": "ChildActorComponent"
                                }
                                try:
                                    child_class = comp.get_editor_property("child_actor_class")
                                    if child_class:
                                        node_info["child_actor_class"] = child_class.get_name()
                                        node_info["child_actor_class_path"] = child_class.get_path_name()
                                        print(f"        ChildActorClass: {child_class.get_name()}")
                                except Exception as e:
                                    print(f"        Error: {e}")

                                # Get relative transform
                                try:
                                    rel_loc = comp.get_editor_property("relative_location")
                                    rel_rot = comp.get_editor_property("relative_rotation")
                                    node_info["relative_location"] = f"({rel_loc.x:.2f}, {rel_loc.y:.2f}, {rel_loc.z:.2f})"
                                    node_info["relative_rotation"] = f"({rel_rot.pitch:.2f}, {rel_rot.yaw:.2f}, {rel_rot.roll:.2f})"
                                    print(f"        RelativeRotation: {node_info['relative_rotation']}")
                                except Exception as e:
                                    pass

                                # Get attachment socket
                                try:
                                    socket = comp.get_attach_socket_name()
                                    if socket and str(socket) != "None":
                                        node_info["attach_socket"] = str(socket)
                                        print(f"        AttachSocket: {socket}")
                                except Exception as e:
                                    pass

                                result["scs_nodes"].append(node_info)

                        # Get the child actor itself
                        for comp in all_comps:
                            if isinstance(comp, unreal.ChildActorComponent):
                                try:
                                    child_actor = comp.get_child_actor()
                                    if child_actor:
                                        print(f"        Child Actor: {child_actor.get_name()} ({child_actor.get_class().get_name()})")
                                except Exception as e:
                                    pass

                        # Destroy the temp actor
                        actor.destroy_actor()
                        print(f"    Destroyed temp actor")
        except Exception as e:
            print(f"  Spawn method failed: {e}")

        all_results[bp_name] = result

    # Save output
    output_file = f"C:/scripts/SLFConversion/migration_cache/scs_weapon_{project_name}.json"
    with open(output_file, 'w') as f:
        json.dump(all_results, f, indent=2)

    print(f"\n{'='*70}")
    print(f"Saved to: {output_file}")
    print(f"{'='*70}")

def extract_scs_node(node):
    """Extract info from an SCS node"""
    info = {}
    try:
        info["name"] = str(node.get_editor_property("internal_variable_name"))
    except:
        info["name"] = node.get_name() if hasattr(node, 'get_name') else "Unknown"

    try:
        comp_template = node.get_editor_property("component_template")
        if comp_template:
            info["class"] = comp_template.get_class().get_name()
            if isinstance(comp_template, unreal.ChildActorComponent):
                try:
                    child_class = comp_template.get_editor_property("child_actor_class")
                    if child_class:
                        info["child_actor_class"] = child_class.get_name()
                except:
                    pass
    except Exception as e:
        info["error"] = str(e)

    return info

if __name__ == "__main__":
    main()
