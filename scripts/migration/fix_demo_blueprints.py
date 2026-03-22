"""
Fix Demo Blueprints - Clear graphs in demo Blueprints that are blocking PIE
"""

import unreal

RESULTS_FILE = "C:/scripts/SLFConversion/fix_demo_results.txt"

def log(msg):
    unreal.log(str(msg))
    with open(RESULTS_FILE, "a") as f:
        f.write(str(msg) + "\n")

# Blueprints with broken graphs that need clearing
BROKEN_DEMO_BLUEPRINTS = [
    "/Game/SoulslikeFramework/Demo/ShowcaseRoom/DemoRoom/Blueprints/B_ShowcaseEnemy_StatDisplay",
    "/Game/SoulslikeFramework/Demo/ShowcaseRoom/DemoRoom/Blueprints/B_Demo_TimeSlider",
]

# AnimBP that needs the AnimGraph Property Access fixed
BROKEN_ANIMBPS = [
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeBossNew",
]

def clear_blueprint_graphs(bp_path):
    """Clear all graphs in a Blueprint"""
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        log(f"[ERROR] Could not load: {bp_path}")
        return False

    log(f"Processing: {bp.get_name()}")

    # Get all graphs
    ubergraph_pages = bp.get_editor_property('ubergraph_pages')
    function_graphs = bp.get_editor_property('function_graphs')

    cleared = 0

    # Clear UberGraph pages (EventGraph, etc.)
    if ubergraph_pages:
        for graph in ubergraph_pages:
            graph_name = graph.get_name()
            nodes = graph.get_editor_property('nodes')
            if nodes:
                node_count = len(nodes)
                nodes.clear()
                log(f"  Cleared {graph_name}: {node_count} nodes removed")
                cleared += node_count

    # Clear function graphs
    if function_graphs:
        for graph in function_graphs:
            graph_name = graph.get_name()
            nodes = graph.get_editor_property('nodes')
            if nodes:
                node_count = len(nodes)
                nodes.clear()
                log(f"  Cleared {graph_name}: {node_count} nodes removed")
                cleared += node_count

    if cleared > 0:
        # Mark dirty and save
        unreal.EditorAssetLibrary.save_asset(bp_path, only_if_is_dirty=False)
        log(f"  [SUCCESS] Saved {bp.get_name()}")
    else:
        log(f"  No nodes to clear")

    return True

def clear_animbp_eventgraph(bp_path):
    """Clear EventGraph in AnimBP but preserve AnimGraph"""
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        log(f"[ERROR] Could not load AnimBP: {bp_path}")
        return False

    log(f"Processing AnimBP: {bp.get_name()}")

    # Get UberGraph pages (EventGraph) - AnimGraph is separate
    ubergraph_pages = bp.get_editor_property('ubergraph_pages')

    cleared = 0
    if ubergraph_pages:
        for graph in ubergraph_pages:
            graph_name = graph.get_name()
            nodes = graph.get_editor_property('nodes')
            if nodes:
                node_count = len(nodes)
                nodes.clear()
                log(f"  Cleared {graph_name}: {node_count} nodes removed")
                cleared += node_count

    # Also clear function graphs in the AnimBP
    function_graphs = bp.get_editor_property('function_graphs')
    if function_graphs:
        for graph in function_graphs:
            graph_name = graph.get_name()
            # Skip AnimGraph-related functions
            if 'AnimGraph' in graph_name:
                log(f"  Skipping AnimGraph function: {graph_name}")
                continue
            nodes = graph.get_editor_property('nodes')
            if nodes:
                node_count = len(nodes)
                nodes.clear()
                log(f"  Cleared {graph_name}: {node_count} nodes removed")
                cleared += node_count

    if cleared > 0:
        unreal.EditorAssetLibrary.save_asset(bp_path, only_if_is_dirty=False)
        log(f"  [SUCCESS] Saved {bp.get_name()}")

    return True

def run():
    # Clear results file
    with open(RESULTS_FILE, "w") as f:
        f.write("")

    log("=" * 60)
    log("Fixing Demo Blueprints")
    log("=" * 60)

    # Fix regular Demo Blueprints
    log("\n--- Clearing Broken Demo Blueprints ---")
    for bp_path in BROKEN_DEMO_BLUEPRINTS:
        clear_blueprint_graphs(bp_path)

    # Fix Demo AnimBPs
    log("\n--- Fixing Demo AnimBPs ---")
    for bp_path in BROKEN_ANIMBPS:
        clear_animbp_eventgraph(bp_path)

    log("\n" + "=" * 60)
    log("Done! Demo Blueprints cleared.")
    log("=" * 60)

run()
