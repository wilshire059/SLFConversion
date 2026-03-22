import unreal

def log(msg):
    print(f"[AnimBPCheck] {msg}")
    unreal.log_warning(f"[AnimBPCheck] {msg}")

log("=" * 70)
log("ABP_SOULSLIKEBOSSNEW GRAPH CHECK")
log("=" * 70)

animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeBossNew"
animbp = unreal.EditorAssetLibrary.load_asset(animbp_path)

if animbp:
    log(f"Loaded: {animbp}")
    log(f"Class: {animbp.get_class().get_name()}")

    # Check if it's an AnimBlueprint
    if hasattr(animbp, 'get_name'):
        log(f"Name: {animbp.get_name()}")

    # Try to get graphs
    try:
        # AnimBlueprint specific properties
        skeleton = animbp.get_editor_property("target_skeleton")
        log(f"Target Skeleton: {skeleton}")
    except Exception as e:
        log(f"Skeleton error: {e}")

    # Check blueprint graphs
    try:
        # UBlueprint has UbergraphPages (EventGraph)
        ubergraph_pages = animbp.get_editor_property("ubergraph_pages")
        log(f"UbergraphPages count: {len(ubergraph_pages) if ubergraph_pages else 0}")
        if ubergraph_pages:
            for i, page in enumerate(ubergraph_pages):
                log(f"  Page {i}: {page.get_name() if page else 'None'}")
                if page:
                    # Get nodes in this graph
                    try:
                        nodes = page.get_editor_property("nodes")
                        log(f"    Nodes: {len(nodes) if nodes else 0}")
                        if nodes:
                            for j, node in enumerate(nodes[:10]):  # First 10 nodes
                                node_class = node.get_class().get_name() if node else "None"
                                log(f"      Node {j}: {node_class}")
                    except Exception as e:
                        log(f"    Nodes error: {e}")
    except Exception as e:
        log(f"UbergraphPages error: {e}")

    # Check function graphs
    try:
        function_graphs = animbp.get_editor_property("function_graphs")
        log(f"FunctionGraphs count: {len(function_graphs) if function_graphs else 0}")
        if function_graphs:
            for i, graph in enumerate(function_graphs):
                log(f"  Graph {i}: {graph.get_name() if graph else 'None'}")
    except Exception as e:
        log(f"FunctionGraphs error: {e}")

else:
    log(f"ERROR: Could not load {animbp_path}")

log("=" * 70)
