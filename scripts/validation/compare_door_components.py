"""
Compare B_Door components between bp_only and SLFConversion projects
"""
import unreal
import json

def export_door_data():
    """Export B_Door component and CDO data"""

    results = {
        "project": "unknown",
        "b_door": None,
        "b_door_demo": None,
        "b_interactable": None
    }

    # Detect which project we're in
    project_name = unreal.Paths.get_project_file_path()
    if "bp_only" in project_name.lower():
        results["project"] = "bp_only"
    elif "slfconversion" in project_name.lower():
        results["project"] = "SLFConversion"

    print(f"\n{'='*60}")
    print(f"PROJECT: {results['project']}")
    print(f"{'='*60}")

    # List of Blueprints to check
    blueprints_to_check = [
        ("/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Door", "b_door"),
        ("/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Door_Demo", "b_door_demo"),
        ("/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Interactable", "b_interactable"),
    ]

    for bp_path, key in blueprints_to_check:
        print(f"\n--- {bp_path} ---")

        bp_data = {
            "path": bp_path,
            "exists": False,
            "parent_class": None,
            "generated_class": None,
            "components": [],
            "cdo_properties": {}
        }

        # Try to load the Blueprint
        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            print(f"  NOT FOUND")
            results[key] = bp_data
            continue

        bp_data["exists"] = True
        print(f"  Found: {bp.get_name()}")

        # Get parent class
        if hasattr(bp, 'parent_class') and bp.parent_class:
            bp_data["parent_class"] = str(bp.parent_class.get_path_name())
            print(f"  Parent Class: {bp_data['parent_class']}")

        # Get generated class
        gen_class = bp.generated_class()
        if gen_class:
            bp_data["generated_class"] = str(gen_class.get_path_name())
            print(f"  Generated Class: {bp_data['generated_class']}")

            # Get CDO
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                print(f"  CDO: {cdo.get_name()}")

                # Try to get some key properties
                try:
                    if hasattr(cdo, 'can_be_traced'):
                        bp_data["cdo_properties"]["can_be_traced"] = cdo.can_be_traced
                    if hasattr(cdo, 'is_activated'):
                        bp_data["cdo_properties"]["is_activated"] = cdo.is_activated
                    if hasattr(cdo, 'interactable_display_name'):
                        bp_data["cdo_properties"]["interactable_display_name"] = str(cdo.interactable_display_name)
                    if hasattr(cdo, 'interaction_text'):
                        bp_data["cdo_properties"]["interaction_text"] = str(cdo.interaction_text)
                except Exception as e:
                    print(f"  Error getting CDO properties: {e}")

        # Get SimpleConstructionScript (SCS) components
        if hasattr(bp, 'simple_construction_script') and bp.simple_construction_script:
            scs = bp.simple_construction_script
            print(f"  SCS: Found")

            # Get all nodes
            if hasattr(scs, 'get_all_nodes'):
                nodes = scs.get_all_nodes()
                print(f"  SCS Nodes: {len(nodes)}")

                for node in nodes:
                    comp_data = {
                        "name": "Unknown",
                        "class": "Unknown",
                        "is_root": False,
                        "parent": None,
                        "properties": {}
                    }

                    if hasattr(node, 'get_variable_name'):
                        comp_data["name"] = str(node.get_variable_name())

                    if hasattr(node, 'component_class') and node.component_class:
                        comp_data["class"] = str(node.component_class.get_name())

                    if hasattr(node, 'component_template') and node.component_template:
                        template = node.component_template
                        comp_data["template_name"] = str(template.get_name())
                        comp_data["template_class"] = str(template.get_class().get_name())

                        # Check for static mesh
                        if hasattr(template, 'static_mesh') and template.static_mesh:
                            comp_data["properties"]["static_mesh"] = str(template.static_mesh.get_path_name())
                        elif hasattr(template, 'get_editor_property'):
                            try:
                                sm = template.get_editor_property('static_mesh')
                                if sm:
                                    comp_data["properties"]["static_mesh"] = str(sm.get_path_name())
                            except:
                                pass

                        # Check visibility
                        if hasattr(template, 'get_editor_property'):
                            try:
                                visible = template.get_editor_property('visible')
                                comp_data["properties"]["visible"] = visible
                            except:
                                pass

                            try:
                                hidden = template.get_editor_property('hidden_in_game')
                                comp_data["properties"]["hidden_in_game"] = hidden
                            except:
                                pass

                    bp_data["components"].append(comp_data)
                    print(f"    Component: {comp_data['name']} ({comp_data.get('template_class', comp_data['class'])})")
                    if "static_mesh" in comp_data.get("properties", {}):
                        print(f"      Mesh: {comp_data['properties']['static_mesh']}")
        else:
            print(f"  SCS: None or not accessible")

        # Also try to spawn an instance and check its components
        print(f"  --- Spawned Instance Components ---")
        try:
            world = unreal.EditorLevelLibrary.get_editor_world()
            if world and gen_class:
                # Spawn temporarily
                spawn_loc = unreal.Vector(0, 0, -10000)  # Far below
                spawn_rot = unreal.Rotator(0, 0, 0)

                actor = unreal.EditorLevelLibrary.spawn_actor_from_class(gen_class, spawn_loc, spawn_rot)
                if actor:
                    print(f"    Spawned: {actor.get_name()}")

                    # Get all components
                    components = actor.get_components_by_class(unreal.ActorComponent)
                    print(f"    Total Components: {len(components)}")

                    for comp in components:
                        comp_info = f"    - {comp.get_name()} ({comp.get_class().get_name()})"

                        # Check for mesh
                        if isinstance(comp, unreal.StaticMeshComponent):
                            sm = comp.static_mesh
                            if sm:
                                comp_info += f" -> {sm.get_name()}"
                            else:
                                comp_info += f" -> NO MESH"

                            # Check visibility
                            visible = comp.is_visible()
                            comp_info += f" [Visible: {visible}]"

                        print(comp_info)

                        # Add to spawned_components
                        spawned_comp = {
                            "name": str(comp.get_name()),
                            "class": str(comp.get_class().get_name()),
                        }
                        if isinstance(comp, unreal.StaticMeshComponent):
                            spawned_comp["static_mesh"] = str(comp.static_mesh.get_path_name()) if comp.static_mesh else None
                            spawned_comp["visible"] = comp.is_visible()

                        if "spawned_components" not in bp_data:
                            bp_data["spawned_components"] = []
                        bp_data["spawned_components"].append(spawned_comp)

                    # Destroy the test actor
                    actor.destroy_actor()
                else:
                    print(f"    Failed to spawn")
        except Exception as e:
            print(f"    Error spawning: {e}")

        results[key] = bp_data

    # Save results
    output_path = f"C:/scripts/slfconversion/migration_cache/door_components_{results['project']}.json"
    with open(output_path, 'w') as f:
        json.dump(results, f, indent=2, default=str)
    print(f"\n\nSaved results to: {output_path}")

    return results

# Run
export_door_data()
