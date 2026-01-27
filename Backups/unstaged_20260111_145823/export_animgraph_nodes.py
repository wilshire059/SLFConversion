# export_animgraph_nodes.py
# Exports all AnimGraph nodes and pins to JSON for validation
# Run on backup project BEFORE renaming variables

import unreal
import json
import os

# Target AnimBlueprints to validate
ANIM_BLUEPRINTS = [
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive",
]

OUTPUT_DIR = "C:/scripts/SLFConversion/migration_cache"

def log(msg):
    print(f"[ExportAnimGraph] {msg}")
    unreal.log(f"[ExportAnimGraph] {msg}")

def get_node_info(node):
    """Extract information from an animation graph node."""
    info = {
        "class": node.get_class().get_name(),
        "name": node.get_name(),
        "node_title": "",
        "pins": [],
        "properties": {},
    }

    # Try to get node title
    try:
        info["node_title"] = str(node.get_editor_property("node_title")) if hasattr(node, "node_title") else ""
    except:
        pass

    # Get all properties we can access
    for prop in node.get_class().properties():
        prop_name = str(prop.get_name())
        try:
            value = node.get_editor_property(prop_name)
            # Convert to string representation for JSON
            if value is not None:
                if isinstance(value, (bool, int, float, str)):
                    info["properties"][prop_name] = value
                elif isinstance(value, unreal.Array):
                    info["properties"][prop_name] = f"Array[{len(value)}]"
                else:
                    info["properties"][prop_name] = str(type(value).__name__)
        except:
            pass

    return info

def get_graph_nodes(graph):
    """Get all nodes from a graph."""
    nodes = []
    try:
        # EdGraph nodes
        for node in graph.nodes:
            node_info = get_node_info(node)
            nodes.append(node_info)
    except Exception as e:
        log(f"  Error getting graph nodes: {e}")
    return nodes

def export_anim_blueprint(asset_path):
    """Export all graph information from an AnimBlueprint."""
    log(f"Exporting: {asset_path}")

    asset = unreal.EditorAssetLibrary.load_asset(asset_path)
    if not asset:
        log(f"  ERROR: Could not load asset")
        return None

    # Get the Blueprint
    bp = None
    if isinstance(asset, unreal.AnimBlueprint):
        bp = asset
    else:
        log(f"  ERROR: Not an AnimBlueprint")
        return None

    # Get parent class safely
    parent_class_name = "None"
    try:
        if hasattr(bp, 'parent_class') and bp.parent_class:
            parent_class_name = str(bp.parent_class.get_name())
        elif hasattr(bp, 'generated_class') and bp.generated_class:
            super_class = bp.generated_class.get_super_class()
            if super_class:
                parent_class_name = str(super_class.get_name())
    except:
        pass

    export_data = {
        "path": asset_path,
        "name": bp.get_name(),
        "parent_class": parent_class_name,
        "graphs": [],
        "variables": [],
    }

    # Export variables
    log(f"  Exporting variables...")
    try:
        # Get generated class for variable info
        gen_class = bp.generated_class
        if gen_class:
            for prop in gen_class.properties():
                prop_name = str(prop.get_name())
                prop_type = str(prop.get_class().get_name())
                export_data["variables"].append({
                    "name": prop_name,
                    "type": prop_type,
                    "has_question_mark": "?" in prop_name,
                })
    except Exception as e:
        log(f"  Error exporting variables: {e}")

    # Export graphs using Blueprint's uber graph pages and function graphs
    log(f"  Exporting graphs...")
    try:
        # Get all graphs from the blueprint
        all_graphs = unreal.BlueprintEditorLibrary.get_graphs(bp)
        for graph in all_graphs:
            graph_name = graph.get_name()
            log(f"    Graph: {graph_name}")

            graph_data = {
                "name": graph_name,
                "schema": str(graph.schema.get_name()) if graph.schema else "None",
                "nodes": [],
            }

            # Get nodes
            for node in graph.nodes:
                node_class = node.get_class().get_name()
                node_data = {
                    "class": node_class,
                    "name": node.get_name(),
                    "guid": "",
                    "comment": "",
                    "properties": {},
                }

                # Special handling for PropertyAccess nodes
                if "PropertyAccess" in node_class:
                    try:
                        # Try to get the path
                        if hasattr(node, "path"):
                            node_data["properties"]["path"] = str(node.get_editor_property("path"))
                        if hasattr(node, "resolved_path"):
                            node_data["properties"]["resolved_path"] = str(node.get_editor_property("resolved_path"))
                    except:
                        pass

                # Get node GUID if available
                try:
                    if hasattr(node, "node_guid"):
                        node_data["guid"] = str(node.get_editor_property("node_guid"))
                except:
                    pass

                # Get node comment if available
                try:
                    if hasattr(node, "node_comment"):
                        node_data["comment"] = str(node.get_editor_property("node_comment"))
                except:
                    pass

                graph_data["nodes"].append(node_data)

            export_data["graphs"].append(graph_data)
            log(f"      {len(graph_data['nodes'])} nodes")

    except Exception as e:
        log(f"  Error exporting graphs: {e}")
        import traceback
        traceback.print_exc()

    return export_data

def main():
    log("=" * 60)
    log("ANIMGRAPH NODE EXPORT")
    log("=" * 60)

    # Create output directory
    os.makedirs(OUTPUT_DIR, exist_ok=True)

    all_exports = {}

    for asset_path in ANIM_BLUEPRINTS:
        export_data = export_anim_blueprint(asset_path)
        if export_data:
            all_exports[asset_path] = export_data

    # Save to JSON
    output_file = os.path.join(OUTPUT_DIR, "animgraph_nodes_before.json")
    with open(output_file, "w") as f:
        json.dump(all_exports, f, indent=2, default=str)

    log(f"")
    log(f"Exported {len(all_exports)} AnimBlueprints to: {output_file}")
    log("=" * 60)

if __name__ == "__main__":
    main()
