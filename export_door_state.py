"""
Export complete door state from both projects for comparison.
This exports B_Door, B_BossDoor, and related AnimBP state.
"""

import unreal
import json
import os

def export_blueprint_graphs(bp_path, output_name):
    """Export all graphs from a Blueprint."""
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_warning(f"Could not load: {bp_path}")
        return None

    result = {
        "path": bp_path,
        "name": bp.get_name(),
        "graphs": [],
        "functions": [],
        "variables": [],
        "components": []
    }

    # Get generated class
    gen_class = bp.generated_class()
    if gen_class:
        result["parent_class"] = str(gen_class.get_class_path_name()) if hasattr(gen_class, 'get_class_path_name') else str(gen_class)

    # Export using BlueprintEditorLibrary
    try:
        # Get all graphs
        graphs = unreal.BlueprintEditorLibrary.get_all_graphs(bp)
        for graph in graphs:
            graph_info = {
                "name": graph.get_name() if graph else "Unknown",
                "nodes": []
            }

            # Try to get nodes from graph
            try:
                nodes = unreal.BlueprintEditorLibrary.get_all_nodes(graph)
                for node in nodes:
                    if node:
                        node_info = {
                            "class": node.get_class().get_name() if node.get_class() else "Unknown",
                            "name": str(node) if node else "Unknown"
                        }
                        graph_info["nodes"].append(node_info)
            except Exception as e:
                graph_info["error"] = str(e)

            result["graphs"].append(graph_info)
    except Exception as e:
        result["graph_error"] = str(e)

    # Spawn to get components
    try:
        world = unreal.EditorLevelLibrary.get_editor_world()
        if world and gen_class:
            actor = unreal.EditorLevelLibrary.spawn_actor_from_class(
                gen_class,
                unreal.Vector(0, 0, 5000),  # High up to avoid collision
                unreal.Rotator(0, 0, 0)
            )
            if actor:
                # Get all components
                components = actor.get_components_by_class(unreal.ActorComponent)
                for comp in components:
                    if comp:
                        comp_info = {
                            "name": comp.get_name(),
                            "class": comp.get_class().get_name() if comp.get_class() else "Unknown"
                        }

                        # Get transform for scene components
                        if isinstance(comp, unreal.SceneComponent):
                            try:
                                comp_info["relative_location"] = {
                                    "x": comp.relative_location.x,
                                    "y": comp.relative_location.y,
                                    "z": comp.relative_location.z
                                }
                                comp_info["relative_scale"] = {
                                    "x": comp.relative_scale3d.x,
                                    "y": comp.relative_scale3d.y,
                                    "z": comp.relative_scale3d.z
                                }
                            except:
                                pass

                        result["components"].append(comp_info)

                actor.destroy_actor()
    except Exception as e:
        result["component_error"] = str(e)

    return result

def export_animbp_state(bp_path, output_name):
    """Export AnimBlueprint state including variables and graph connections."""
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_warning(f"Could not load AnimBP: {bp_path}")
        return None

    result = {
        "path": bp_path,
        "name": bp.get_name(),
        "type": "AnimBlueprint",
        "variables": [],
        "graphs": []
    }

    try:
        # Get skeleton class
        if hasattr(bp, 'target_skeleton'):
            result["target_skeleton"] = str(bp.target_skeleton) if bp.target_skeleton else None

        # Get parent class
        gen_class = bp.generated_class()
        if gen_class:
            parent = gen_class.get_super_class() if hasattr(gen_class, 'get_super_class') else None
            result["parent_class"] = str(parent) if parent else None

        # Export graphs
        graphs = unreal.BlueprintEditorLibrary.get_all_graphs(bp)
        for graph in graphs:
            if graph:
                graph_info = {
                    "name": graph.get_name(),
                    "node_count": 0,
                    "nodes": []
                }

                try:
                    nodes = unreal.BlueprintEditorLibrary.get_all_nodes(graph)
                    graph_info["node_count"] = len(nodes) if nodes else 0

                    for node in nodes[:50]:  # Limit to first 50 nodes
                        if node:
                            node_class = node.get_class()
                            node_info = {
                                "class": node_class.get_name() if node_class else "Unknown"
                            }

                            # Try to get node title/name
                            try:
                                node_info["title"] = str(node)
                            except:
                                pass

                            graph_info["nodes"].append(node_info)
                except Exception as e:
                    graph_info["error"] = str(e)

                result["graphs"].append(graph_info)

    except Exception as e:
        result["error"] = str(e)

    return result

def main():
    """Export door and AnimBP state."""

    output_dir = "C:/scripts/SLFConversion/migration_cache/door_comparison"
    os.makedirs(output_dir, exist_ok=True)

    unreal.log_warning("=" * 70)
    unreal.log_warning("EXPORTING DOOR AND ANIMBP STATE")
    unreal.log_warning("=" * 70)

    # Blueprints to export
    blueprints = [
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Door",
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_BossDoor",
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Door_Demo",
    ]

    animbps = [
        "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive",
    ]

    all_data = {"blueprints": {}, "animbps": {}}

    # Export Blueprints
    for bp_path in blueprints:
        unreal.log_warning(f"Exporting: {bp_path}")
        data = export_blueprint_graphs(bp_path, bp_path.split("/")[-1])
        if data:
            all_data["blueprints"][bp_path.split("/")[-1]] = data
            unreal.log_warning(f"  Graphs: {len(data.get('graphs', []))}")
            unreal.log_warning(f"  Components: {len(data.get('components', []))}")

    # Export AnimBPs
    for bp_path in animbps:
        unreal.log_warning(f"Exporting AnimBP: {bp_path}")
        data = export_animbp_state(bp_path, bp_path.split("/")[-1])
        if data:
            all_data["animbps"][bp_path.split("/")[-1]] = data
            unreal.log_warning(f"  Graphs: {len(data.get('graphs', []))}")

    # Save to JSON
    output_file = os.path.join(output_dir, "door_state_SLFConversion.json")
    with open(output_file, 'w') as f:
        json.dump(all_data, f, indent=2, default=str)

    unreal.log_warning(f"\nSaved to: {output_file}")
    unreal.log_warning("=" * 70)

if __name__ == "__main__":
    main()
