# export_complete_animbp.py
# COMPREHENSIVE export of ALL AnimBP graphs - no shortcuts
# Exports every graph, every node, every pin with full connection details

import unreal
import json
import os

def log(msg):
    print(f"[FullExport] {msg}")
    unreal.log_warning(f"[FullExport] {msg}")

ANIMBP_PATH = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"
OUTPUT_PATH = "C:/scripts/SLFConversion/migration_cache/animbp_complete_current.json"

def export_pin_full(pin):
    """Export complete pin information including all connections"""
    if not pin:
        return None

    pin_data = {
        "name": str(pin.pin_name) if hasattr(pin, 'pin_name') else "unknown",
        "direction": "INPUT" if pin.direction == unreal.EdGraphPinDirection.EGPD_INPUT else "OUTPUT",
        "type_category": str(pin.pin_type.pin_category) if hasattr(pin, 'pin_type') else "",
        "type_subcategory": str(pin.pin_type.pin_sub_category) if hasattr(pin, 'pin_type') else "",
        "default_value": str(pin.default_value) if hasattr(pin, 'default_value') and pin.default_value else "",
        "is_connected": False,
        "connections": []
    }

    # Get all connections
    if hasattr(pin, 'linked_to') and pin.linked_to:
        pin_data["is_connected"] = len(pin.linked_to) > 0
        for linked_pin in pin.linked_to:
            if linked_pin:
                owner_node = linked_pin.get_owning_node() if hasattr(linked_pin, 'get_owning_node') else None
                conn = {
                    "pin_name": str(linked_pin.pin_name) if hasattr(linked_pin, 'pin_name') else "?",
                    "owner_node_name": owner_node.get_name() if owner_node else "?",
                    "owner_node_class": owner_node.get_class().get_name() if owner_node else "?"
                }
                # Try to get node title
                if owner_node and hasattr(owner_node, 'get_node_title'):
                    try:
                        conn["owner_node_title"] = str(owner_node.get_node_title(unreal.NodeTitleType.FULL_TITLE))
                    except:
                        pass
                pin_data["connections"].append(conn)

    return pin_data

def export_node_full(node):
    """Export complete node information including all pins"""
    if not node:
        return None

    node_data = {
        "name": node.get_name() if hasattr(node, 'get_name') else str(node),
        "class": node.get_class().get_name() if hasattr(node, 'get_class') else "unknown",
        "title": "",
        "position_x": node.node_pos_x if hasattr(node, 'node_pos_x') else 0,
        "position_y": node.node_pos_y if hasattr(node, 'node_pos_y') else 0,
        "pins": [],
        "extra_info": {}
    }

    # Get node title
    if hasattr(node, 'get_node_title'):
        try:
            title = str(node.get_node_title(unreal.NodeTitleType.FULL_TITLE))
            node_data["title"] = title.replace("\n", " | ")
        except:
            pass

    # Get all pins
    pins = []
    if hasattr(node, 'pins'):
        pins = list(node.pins)

    for pin in pins:
        pin_data = export_pin_full(pin)
        if pin_data:
            node_data["pins"].append(pin_data)

    # Special handling for specific node types
    node_class = node_data["class"]

    # VariableGet nodes - get variable name
    if "VariableGet" in node_class:
        if hasattr(node, 'variable_reference'):
            var_ref = node.variable_reference
            if hasattr(var_ref, 'member_name'):
                node_data["extra_info"]["variable_name"] = str(var_ref.member_name)
            if hasattr(var_ref, 'member_parent'):
                parent = var_ref.member_parent
                node_data["extra_info"]["variable_parent"] = str(parent) if parent else "Self"

    # State nodes
    if "AnimStateNode" in node_class:
        if hasattr(node, 'bound_graph'):
            node_data["extra_info"]["bound_graph"] = node.bound_graph.get_name() if node.bound_graph else None

    # Transition nodes
    if "TransitionNode" in node_class:
        if hasattr(node, 'bound_graph'):
            node_data["extra_info"]["bound_graph"] = node.bound_graph.get_name() if node.bound_graph else None

    return node_data

def export_graph_full(graph, depth=0):
    """Export complete graph information including all nodes and nested graphs"""
    if not graph:
        return None

    graph_data = {
        "name": graph.get_name() if hasattr(graph, 'get_name') else str(graph),
        "class": graph.get_class().get_name() if hasattr(graph, 'get_class') else "unknown",
        "node_count": 0,
        "nodes": [],
        "subgraphs": []
    }

    # Get all nodes
    nodes = []
    if hasattr(graph, 'nodes'):
        nodes = list(graph.nodes)

    graph_data["node_count"] = len(nodes)

    for node in nodes:
        node_data = export_node_full(node)
        if node_data:
            graph_data["nodes"].append(node_data)

            # Check for nested/bound graphs
            if hasattr(node, 'bound_graph') and node.bound_graph and depth < 3:
                subgraph = export_graph_full(node.bound_graph, depth + 1)
                if subgraph:
                    graph_data["subgraphs"].append({
                        "parent_node": node_data["name"],
                        "graph": subgraph
                    })

    return graph_data

def main():
    log("=" * 80)
    log("COMPREHENSIVE ANIMBP EXPORT - ALL GRAPHS, ALL NODES, ALL PINS")
    log("=" * 80)

    bp = unreal.load_asset(ANIMBP_PATH)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    export_data = {
        "asset_path": ANIMBP_PATH,
        "asset_name": bp.get_name(),
        "graphs": [],
        "summary": {
            "total_graphs": 0,
            "total_nodes": 0,
            "graph_names": []
        }
    }

    # Get parent class info
    if hasattr(bp, 'generated_class') and bp.generated_class:
        gen_class = bp.generated_class
        if hasattr(gen_class, 'get_super_class'):
            parent = gen_class.get_super_class()
            export_data["parent_class"] = parent.get_name() if parent else "None"

    # Get ALL graphs
    all_graphs = []

    # Method 1: UberGraphPages (EventGraph, etc.)
    if hasattr(bp, 'ubergraph_pages'):
        for g in bp.ubergraph_pages:
            if g:
                all_graphs.append(("UberGraph", g))

    # Method 2: FunctionGraphs
    if hasattr(bp, 'function_graphs'):
        for g in bp.function_graphs:
            if g:
                all_graphs.append(("FunctionGraph", g))

    # Method 3: AnimBP specific - anim_graphs (if AnimBlueprint)
    if hasattr(bp, 'anim_graphs'):
        for g in bp.anim_graphs:
            if g:
                all_graphs.append(("AnimGraph", g))

    log(f"Found {len(all_graphs)} top-level graphs")

    total_nodes = 0

    for graph_type, graph in all_graphs:
        graph_name = graph.get_name() if hasattr(graph, 'get_name') else str(graph)
        log(f"  Exporting {graph_type}: {graph_name}")

        graph_data = export_graph_full(graph)
        if graph_data:
            graph_data["graph_type"] = graph_type
            export_data["graphs"].append(graph_data)
            export_data["summary"]["graph_names"].append(graph_name)
            total_nodes += graph_data["node_count"]

            # Count nodes in subgraphs
            for subgraph_entry in graph_data.get("subgraphs", []):
                if "graph" in subgraph_entry:
                    total_nodes += subgraph_entry["graph"].get("node_count", 0)

    export_data["summary"]["total_graphs"] = len(all_graphs)
    export_data["summary"]["total_nodes"] = total_nodes

    log(f"Total graphs: {len(all_graphs)}")
    log(f"Total nodes: {total_nodes}")

    # Save to JSON
    os.makedirs(os.path.dirname(OUTPUT_PATH), exist_ok=True)
    with open(OUTPUT_PATH, 'w', encoding='utf-8') as f:
        json.dump(export_data, f, indent=2, default=str)

    log(f"Saved to {OUTPUT_PATH}")

    # Print graph names for reference
    log("\nGraph names found:")
    for name in export_data["summary"]["graph_names"]:
        log(f"  - {name}")

    log("\n[SUCCESS] Complete export finished")

if __name__ == "__main__":
    main()
