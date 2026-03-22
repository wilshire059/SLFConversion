"""
Surgical reparenting of ABP_SoulslikeCharacter_Additive to C++ class.
ONLY clears EventGraph, preserves AnimGraph and LL implementation graphs.
"""
import unreal

ANIMBP_PATH = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"
CPP_PARENT = "/Script/SLFConversion.ABP_SoulslikeCharacter_Additive"

def surgical_reparent():
    # Load the AnimBP
    bp = unreal.load_asset(ANIMBP_PATH)
    if not bp:
        print(f"ERROR: Could not load {ANIMBP_PATH}")
        return False

    print(f"Loaded AnimBP: {bp.get_name()}")

    # Load target C++ parent class
    cpp_class = unreal.load_class(None, CPP_PARENT)
    if not cpp_class:
        print(f"ERROR: Could not load C++ class {CPP_PARENT}")
        return False

    print(f"Target C++ parent: {cpp_class.get_name()}")

    # Reparent using BlueprintEditorLibrary
    print("Reparenting to C++ class...")
    result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, cpp_class)
    print(f"Reparent result: {result}")

    # Clear ONLY the EventGraph - preserve AnimGraph and other graphs
    all_graphs = unreal.BlueprintEditorLibrary.get_all_graphs(bp)
    print(f"Found {len(all_graphs)} graphs")

    for graph in all_graphs:
        graph_name = graph.get_name()
        print(f"  Graph: {graph_name}")

        # Only clear EventGraph - this contains the Blueprint logic
        # AnimGraph, LL implementations, etc. must be preserved
        if graph_name == "EventGraph":
            print(f"    Clearing EventGraph nodes...")
            nodes = unreal.BlueprintEditorLibrary.get_all_nodes(graph)
            print(f"    Found {len(nodes)} nodes to remove")
            for node in nodes:
                unreal.BlueprintEditorLibrary.remove_node(node)
            print(f"    EventGraph cleared")
        else:
            print(f"    Preserving {graph_name}")

    # Compile the blueprint
    print("Compiling blueprint...")
    unreal.BlueprintEditorLibrary.compile_blueprint(bp)
    print("Blueprint compiled")

    # Save
    unreal.EditorAssetLibrary.save_asset(ANIMBP_PATH)
    print(f"Saved {ANIMBP_PATH}")

    return True

if __name__ == "__main__":
    success = surgical_reparent()
    print(f"\nResult: {'SUCCESS' if success else 'FAILED'}")
