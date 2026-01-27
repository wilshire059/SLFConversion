"""
Clear Blueprint function graphs for LevelUp widgets.
After reparenting to C++, the Blueprint function bodies should be empty
so the C++ _Implementation methods are called.
"""

import unreal

WIDGETS_TO_CLEAR = [
    "/Game/SoulslikeFramework/Widgets/RestMenu/LevelUpMenu/W_StatEntry_LevelUp",
    "/Game/SoulslikeFramework/Widgets/RestMenu/LevelUpMenu/W_StatBlock_LevelUp",
    "/Game/SoulslikeFramework/Widgets/RestMenu/LevelUpMenu/W_LevelUp",
]

def clear_widget_functions():
    """Clear function graphs in LevelUp widgets."""

    for widget_path in WIDGETS_TO_CLEAR:
        print(f"\n{'='*60}")
        print(f"Processing: {widget_path}")
        print('='*60)

        # Load the widget Blueprint
        bp = unreal.EditorAssetLibrary.load_asset(widget_path)
        if not bp:
            print(f"  ERROR: Could not load {widget_path}")
            continue

        print(f"  Loaded: {bp.get_name()}")

        # Clear function graphs using BlueprintEditorLibrary
        try:
            # Get all function graphs
            function_graphs = unreal.BlueprintEditorLibrary.get_function_graphs(bp)
            print(f"  Found {len(function_graphs)} function graphs")

            for graph in function_graphs:
                graph_name = graph.get_name()
                print(f"    Clearing function: {graph_name}")

                # Get all nodes in the graph
                nodes = list(unreal.BlueprintEditorLibrary.get_nodes_of_graph(graph))
                print(f"      Nodes before clearing: {len(nodes)}")

                # Remove all nodes except the function entry node
                removed_count = 0
                for node in nodes:
                    node_class = node.get_class().get_name()
                    if 'FunctionEntry' not in node_class and 'FunctionResult' not in node_class:
                        try:
                            unreal.BlueprintEditorLibrary.remove_node(bp, node)
                            removed_count += 1
                        except:
                            pass  # Some nodes may not be removable

                print(f"      Removed {removed_count} nodes")

        except Exception as e:
            print(f"  Warning: Could not process function graphs: {e}")

        # Clear event graph
        try:
            event_graphs = unreal.BlueprintEditorLibrary.get_event_graphs(bp)
            print(f"  Found {len(event_graphs)} event graphs")

            for graph in event_graphs:
                graph_name = graph.get_name()
                print(f"    Clearing event graph: {graph_name}")

                nodes = list(unreal.BlueprintEditorLibrary.get_nodes_of_graph(graph))
                print(f"      Nodes before clearing: {len(nodes)}")

                # Remove all nodes
                removed_count = 0
                for node in nodes:
                    try:
                        unreal.BlueprintEditorLibrary.remove_node(bp, node)
                        removed_count += 1
                    except:
                        pass

                print(f"      Removed {removed_count} nodes")

        except Exception as e:
            print(f"  Warning: Could not process event graphs: {e}")

        # Compile and save
        try:
            unreal.BlueprintEditorLibrary.compile_blueprint(bp)
            print(f"  Compiled successfully")
        except Exception as e:
            print(f"  Warning: Compile issue: {e}")

        try:
            unreal.EditorAssetLibrary.save_asset(widget_path, only_if_is_dirty=False)
            print(f"  Saved successfully")
        except Exception as e:
            print(f"  Warning: Save issue: {e}")

if __name__ == "__main__":
    clear_widget_functions()
    print("\n" + "="*60)
    print("DONE - LevelUp widget functions cleared")
    print("="*60)
