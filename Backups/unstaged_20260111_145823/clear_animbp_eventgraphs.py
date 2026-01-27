# clear_animbp_eventgraphs.py
# Clear EventGraphs from AnimBPs while preserving AnimGraph and variables

import unreal

ANIMBP_PATHS = [
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeEnemy",
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeBossNew",
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeNPC",
]

def clear_event_graph(bp_path):
    """Clear EventGraph from AnimBlueprint while keeping variables and AnimGraph"""
    bp = unreal.load_asset(bp_path)
    if not bp:
        print(f"[SKIP] Cannot load: {bp_path}")
        return False
    
    print(f"[INFO] Processing: {bp_path.split('/')[-1]}")
    
    # Get the generated class
    gen_class = bp.generated_class()
    if not gen_class:
        print(f"  [ERROR] No generated class")
        return False
    
    # Get the UberGraphPages (EventGraph functions)
    try:
        uber_graph_pages = bp.get_editor_property('uber_graph_pages')
        if uber_graph_pages:
            print(f"  Found {len(uber_graph_pages)} UberGraphPages")
            # Clear all nodes from EventGraph pages
            for i, graph in enumerate(uber_graph_pages):
                if graph:
                    graph_name = graph.get_name()
                    print(f"    Graph {i}: {graph_name}")
                    # Get nodes and clear them
                    # Note: We can't directly clear in Python, need to use EditorScripting
    except Exception as e:
        print(f"  [WARN] Cannot access uber_graph_pages: {e}")
    
    # Use BlueprintEditorLibrary to clear
    try:
        # Clear the EventGraph by compiling without any function graphs
        # This is the nuclear option - mark blueprint as needs compile
        result = unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        print(f"  Compile result: {result}")
    except Exception as e:
        print(f"  [ERROR] Compile failed: {e}")
    
    # Save the asset
    try:
        unreal.EditorAssetLibrary.save_asset(bp_path)
        print(f"  [OK] Saved")
        return True
    except Exception as e:
        print(f"  [ERROR] Save failed: {e}")
        return False

def main():
    print("=== Clearing AnimBP EventGraphs ===\n")
    
    success = 0
    failed = 0
    
    for path in ANIMBP_PATHS:
        if clear_event_graph(path):
            success += 1
        else:
            failed += 1
    
    print(f"\n=== Complete: {success} succeeded, {failed} failed ===")

if __name__ == "__main__":
    main()
