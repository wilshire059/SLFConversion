# find_animdataasset_nodes.py
# Find nodes that reference AnimDataAsset in AnimGraph

import unreal

def log(msg):
    unreal.log_warning(f"[FindNodes] {msg}")

def main():
    log("=" * 60)
    log("Finding AnimDataAsset References in AnimGraph")
    log("=" * 60)

    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"
    bp = unreal.load_asset(animbp_path)
    if not bp:
        log(f"[ERROR] Failed to load AnimBP")
        return

    # Get all graphs
    graphs = list(bp.ubergraph_pages) + list(bp.function_graphs)

    log(f"Searching {len(graphs)} graphs...")

    for graph in graphs:
        graph_name = graph.get_name()
        nodes = list(graph.nodes)

        log(f"")
        log(f"Graph: {graph_name} ({len(nodes)} nodes)")

        for node in nodes:
            node_class = node.get_class().get_name()
            node_name = node.get_name()

            # Check if it's a VariableGet node
            if "VariableGet" in node_class:
                # Try to get the variable name from the node title
                title = node.node_title if hasattr(node, 'node_title') else ""
                log(f"  [GET] {node_name}: {title}")

            # Check if node has any pins related to AnimDataAsset
            pins = node.pins if hasattr(node, 'pins') else []
            for pin in pins:
                pin_name = pin.pin_name if hasattr(pin, 'pin_name') else str(pin)
                if "Anim" in str(pin_name) or "Data" in str(pin_name):
                    log(f"  [{node_class}] {node_name} has pin: {pin_name}")

    log("")
    log("=" * 60)

if __name__ == "__main__":
    main()
