import unreal

def clear_blueprint_graphs(bp):
    """Clear all function/event graphs from a Blueprint, preserving variables and SCS components"""
    if not bp:
        return False

    gen_class = bp.generated_class()
    if not gen_class:
        unreal.log(f"  No generated class")
        return False

    bp_name = bp.get_name()

    # Get the uber graph frames (event graphs) and function graphs
    try:
        # Access internal graphs
        uber_graph_pages = bp.get_editor_property('ubergraph_pages')
        function_graphs = bp.get_editor_property('function_graphs')

        unreal.log(f"  Found {len(uber_graph_pages)} uber graphs, {len(function_graphs)} function graphs")

        # Clear nodes from uber graphs (EventGraphs)
        cleared_count = 0
        for graph in uber_graph_pages:
            nodes = graph.get_editor_property('nodes')
            if nodes:
                node_count = len(nodes)
                nodes.clear()
                cleared_count += node_count
                unreal.log(f"    Cleared {node_count} nodes from uber graph {graph.get_name()}")

        # Clear function graphs
        for graph in function_graphs:
            func_name = graph.get_name()
            nodes = graph.get_editor_property('nodes')
            if nodes:
                node_count = len(nodes)
                nodes.clear()
                cleared_count += node_count
                unreal.log(f"    Cleared {node_count} nodes from function graph {func_name}")

        unreal.log(f"  Total cleared: {cleared_count} nodes")
        return True

    except Exception as e:
        unreal.log(f"  Error clearing graphs: {e}")
        return False


def clear_and_reparent():
    """Clear Blueprint graphs and reparent to C++ classes"""

    reparent_list = [
        {
            "bp_path": "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Interactable",
            "cpp_class": "/Script/SLFConversion.B_Interactable"
        },
        {
            "bp_path": "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_RestingPoint",
            "cpp_class": "/Script/SLFConversion.B_RestingPoint"
        },
    ]

    for item in reparent_list:
        bp_path = item["bp_path"]
        cpp_class_path = item["cpp_class"]

        unreal.log(f"=== Processing: {bp_path} ===")

        # Load Blueprint
        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            unreal.log(f"  ERROR: Could not load Blueprint")
            continue

        # Clear Blueprint graphs
        unreal.log(f"  Clearing Blueprint graphs...")
        clear_blueprint_graphs(bp)

        # Mark as modified
        unreal.EditorAssetLibrary.checkout_asset(bp_path)

        # Save first before reparenting
        try:
            result = unreal.EditorAssetLibrary.save_asset(bp_path, only_if_is_dirty=False)
            unreal.log(f"  Pre-reparent save: {result}")
        except Exception as e:
            unreal.log(f"  Pre-reparent save error: {e}")

    # Second pass: load again and reparent
    for item in reparent_list:
        bp_path = item["bp_path"]
        cpp_class_path = item["cpp_class"]

        unreal.log(f"=== Reparenting: {bp_path} ===")

        # Load Blueprint fresh
        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            unreal.log(f"  ERROR: Could not load Blueprint")
            continue

        # Load C++ class
        cpp_class = unreal.load_class(None, cpp_class_path)
        if not cpp_class:
            unreal.log(f"  ERROR: Could not load C++ class: {cpp_class_path}")
            continue

        unreal.log(f"  Blueprint: {bp.get_name()}, Target C++ Class: {cpp_class.get_name()}")

        # Reparent
        try:
            result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, cpp_class)
            unreal.log(f"  Reparent result: {result}")
        except Exception as e:
            unreal.log(f"  Reparent error: {e}")
            continue

        # Compile
        try:
            unreal.BlueprintEditorLibrary.compile_blueprint(bp)
            unreal.log(f"  Compiled")
        except Exception as e:
            unreal.log(f"  Compile error: {e}")

        # Save
        try:
            result = unreal.EditorAssetLibrary.save_asset(bp_path, only_if_is_dirty=False)
            unreal.log(f"  Save result: {result}")
        except Exception as e:
            unreal.log(f"  Save error: {e}")

    unreal.log("=== Done ===")

if __name__ == "__main__":
    clear_and_reparent()
