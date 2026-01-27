# export_animbp_detailed.py
# Comprehensive node-by-node, pin-by-pin export of AnimBP transition graphs

import unreal
import json
import os

def log(msg):
    print(f"[Export] {msg}")
    unreal.log_warning(f"[Export] {msg}")

ANIMBP_PATH = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"
OUTPUT_PATH = "C:/scripts/SLFConversion/migration_cache/animbp_detailed_current.json"

def get_pin_info(pin):
    """Extract detailed pin information"""
    info = {
        "name": str(pin.pin_name) if hasattr(pin, 'pin_name') else "?",
        "direction": "Input" if hasattr(pin, 'direction') and pin.direction == unreal.EdGraphPinDirection.EGPD_INPUT else "Output",
        "type": str(pin.pin_type.pin_category) if hasattr(pin, 'pin_type') else "?",
        "sub_type": str(pin.pin_type.pin_sub_category) if hasattr(pin, 'pin_type') else "",
        "default_value": str(pin.default_value) if hasattr(pin, 'default_value') else "",
        "linked_count": len(pin.linked_to) if hasattr(pin, 'linked_to') else 0,
        "linked_to": []
    }

    # Get linked node info
    if hasattr(pin, 'linked_to') and pin.linked_to:
        for linked_pin in pin.linked_to:
            if linked_pin:
                owner = linked_pin.get_owning_node() if hasattr(linked_pin, 'get_owning_node') else None
                info["linked_to"].append({
                    "pin_name": str(linked_pin.pin_name) if hasattr(linked_pin, 'pin_name') else "?",
                    "owner_node": owner.get_name() if owner else "?"
                })

    return info

def get_node_info(node):
    """Extract detailed node information"""
    info = {
        "name": node.get_name() if hasattr(node, 'get_name') else str(node),
        "class": node.get_class().get_name() if hasattr(node, 'get_class') else "?",
        "title": "",
        "pins": []
    }

    # Try to get node title
    if hasattr(node, 'get_node_title'):
        try:
            info["title"] = str(node.get_node_title(unreal.NodeTitleType.FULL_TITLE))
        except:
            pass

    # Get all pins
    pins = []
    if hasattr(node, 'pins'):
        pins = node.pins
    elif hasattr(node, 'get_all_pins'):
        pins = node.get_all_pins()

    for pin in pins:
        if pin:
            info["pins"].append(get_pin_info(pin))

    # Special handling for VariableGet nodes
    if "VariableGet" in info["class"] or "K2Node_VariableGet" in info["class"]:
        if hasattr(node, 'variable_reference'):
            var_ref = node.variable_reference
            if hasattr(var_ref, 'member_name'):
                info["variable_name"] = str(var_ref.member_name)
            if hasattr(var_ref, 'member_parent'):
                info["variable_parent"] = str(var_ref.member_parent) if var_ref.member_parent else "Self"

    return info

def export_graph(graph):
    """Export all nodes and pins from a graph"""
    graph_info = {
        "name": graph.get_name() if hasattr(graph, 'get_name') else str(graph),
        "class": graph.get_class().get_name() if hasattr(graph, 'get_class') else "?",
        "nodes": []
    }

    nodes = []
    if hasattr(graph, 'nodes'):
        nodes = graph.nodes
    elif hasattr(graph, 'get_nodes'):
        nodes = graph.get_nodes()

    for node in nodes:
        if node:
            graph_info["nodes"].append(get_node_info(node))

    return graph_info

def main():
    log("=" * 70)
    log("EXPORTING DETAILED ANIMBP STATE (CURRENT)")
    log("=" * 70)

    bp = unreal.load_asset(ANIMBP_PATH)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    # Get parent class info
    parent_class_name = "Unknown"
    if hasattr(bp, 'blueprint_generated_class') and bp.blueprint_generated_class:
        gen_class = bp.blueprint_generated_class
        if hasattr(gen_class, 'get_super_class'):
            parent = gen_class.get_super_class()
            if parent:
                parent_class_name = parent.get_name()

    export_data = {
        "asset_path": ANIMBP_PATH,
        "parent_class": parent_class_name,
        "graphs": []
    }

    # Get all graphs
    all_graphs = []
    if hasattr(bp, 'get_all_graphs'):
        all_graphs = bp.get_all_graphs()

    log(f"Found {len(all_graphs)} graphs")

    # Export each graph, focusing on transition graphs
    transition_count = 0
    for graph in all_graphs:
        if not graph:
            continue

        graph_name = graph.get_name() if hasattr(graph, 'get_name') else str(graph)

        # Export all transition graphs in detail
        if "Transition" in graph_name or "IDLE" in graph_name or "CYCLE" in graph_name:
            transition_count += 1
            graph_data = export_graph(graph)
            export_data["graphs"].append(graph_data)
            log(f"  Exported: {graph_name} ({len(graph_data['nodes'])} nodes)")

    log(f"Exported {transition_count} transition/state graphs")

    # Save to JSON
    os.makedirs(os.path.dirname(OUTPUT_PATH), exist_ok=True)
    with open(OUTPUT_PATH, 'w') as f:
        json.dump(export_data, f, indent=2)

    log(f"Saved to {OUTPUT_PATH}")
    log("[SUCCESS] Export complete")

if __name__ == "__main__":
    main()
