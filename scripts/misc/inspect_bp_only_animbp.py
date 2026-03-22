# inspect_bp_only_animbp.py
# Run this in the bp_only project to see original connections

import unreal

def log(msg):
    print(f"[Inspect] {msg}")
    unreal.log_warning(f"[Inspect] {msg}")

def main():
    log("=" * 70)
    log("INSPECTING ORIGINAL bp_only ANIMBP")
    log("=" * 70)

    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

    bp = unreal.load_asset(animbp_path)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    # Get all graphs
    if hasattr(bp, 'get_all_graphs'):
        all_graphs = bp.get_all_graphs()
    else:
        all_graphs = []
        if hasattr(bp, 'uberraph_pages'):
            all_graphs.extend(bp.uberraph_pages)

    log(f"Total graphs: {len(all_graphs)}")

    # Find transition graphs and inspect connections
    transition_count = 0
    for graph in all_graphs:
        graph_name = graph.get_name() if hasattr(graph, 'get_name') else str(graph)
        if "Transition" in graph_name or "AnimationTransitionGraph" in graph_name:
            transition_count += 1
            if transition_count <= 5:  # Only show first 5
                log(f"\n=== {graph_name} ===")
                nodes = graph.get_nodes() if hasattr(graph, 'get_nodes') else []
                for node in nodes:
                    node_name = node.get_node_title() if hasattr(node, 'get_node_title') else str(node)
                    log(f"  Node: {node_name}")

                    # Try to get pins
                    if hasattr(node, 'node_get_pins'):
                        pins = node.node_get_pins()
                        for pin in pins:
                            pin_name = pin.pin_name if hasattr(pin, 'pin_name') else "?"
                            direction = "IN" if hasattr(pin, 'direction') and pin.direction == 0 else "OUT"
                            linked = pin.linked_to if hasattr(pin, 'linked_to') else []
                            linked_count = len(linked) if linked else 0
                            log(f"    Pin {direction}: {pin_name} (linked: {linked_count})")

if __name__ == "__main__":
    main()
