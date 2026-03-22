# migrate_container_only.py
# Targeted migration of B_Container only

import unreal

BP_CONTAINER_PATH = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Container"
CPP_CLASS_PATH = "/Script/SLFConversion.SLFContainerBase"

def find_and_load_bp(path):
    """Load Blueprint asset"""
    return unreal.load_asset(path)

def clear_blueprint_nodes(bp):
    """Clear all nodes from a Blueprint"""
    if not bp:
        return 0

    cleared = 0

    # Clear uber graph pages (EventGraphs)
    try:
        uber_graphs = bp.get_editor_property('ubergraph_pages')
        for graph in uber_graphs:
            try:
                nodes = graph.get_editor_property('nodes')
                if nodes:
                    cleared += len(nodes)
                    nodes.clear()
            except:
                pass
    except:
        pass

    # Clear function graphs
    try:
        func_graphs = bp.get_editor_property('function_graphs')
        for graph in func_graphs:
            try:
                nodes = graph.get_editor_property('nodes')
                if nodes:
                    cleared += len(nodes)
                    nodes.clear()
            except:
                pass
    except:
        pass

    # Clear delegate signature graphs
    try:
        delegate_graphs = bp.get_editor_property('delegate_signature_graphs')
        for graph in delegate_graphs:
            try:
                nodes = graph.get_editor_property('nodes')
                if nodes:
                    cleared += len(nodes)
                    nodes.clear()
            except:
                pass
    except:
        pass

    return cleared


def migrate_container():
    """Run B_Container migration"""
    print("=" * 60)
    print("MIGRATING B_Container")
    print("=" * 60)

    # Phase 1: Load and clear
    print("\nPhase 1: Load and clear Blueprint")
    bp = find_and_load_bp(BP_CONTAINER_PATH)
    if not bp:
        print(f"ERROR: Could not load {BP_CONTAINER_PATH}")
        return False

    print(f"  Loaded: {bp.get_name()}")

    # Clear Blueprint nodes using SLFAutomationLibrary (preserves SCS components)
    try:
        lib = unreal.SLFAutomationLibrary()
        result = lib.clear_graphs_keep_variables_no_compile(bp)
        print(f"  Clear graphs result: {result}")
    except Exception as e:
        print(f"  Clear graphs exception: {e}")
        # Fallback to manual clearing
        cleared = clear_blueprint_nodes(bp)
        print(f"  Cleared {cleared} nodes (fallback)")

    # Phase 2: Save cleared Blueprint
    print("\nPhase 2: Save cleared Blueprint")
    try:
        result = unreal.EditorAssetLibrary.save_asset(BP_CONTAINER_PATH, only_if_is_dirty=False)
        print(f"  Save result: {result}")
    except Exception as e:
        print(f"  Save error: {e}")

    # Phase 3: Reload
    print("\nPhase 3: Reload Blueprint")
    bp = find_and_load_bp(BP_CONTAINER_PATH)
    if not bp:
        print("ERROR: Could not reload Blueprint")
        return False

    # Phase 4: Load C++ class and reparent
    print("\nPhase 4: Load C++ class and reparent")
    cpp_class = unreal.load_class(None, CPP_CLASS_PATH)
    if not cpp_class:
        print(f"ERROR: Could not load {CPP_CLASS_PATH}")
        return False

    print(f"  C++ class: {cpp_class.get_name()}")

    try:
        result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, cpp_class)
        print(f"  Reparent result: {result}")
    except Exception as e:
        print(f"  Reparent error: {e}")
        return False

    # Phase 5: Compile
    print("\nPhase 5: Compile")
    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        print("  Compiled successfully")
    except Exception as e:
        print(f"  Compile note: {e}")

    # Phase 6: Final save
    print("\nPhase 6: Final save")
    try:
        result = unreal.EditorAssetLibrary.save_asset(BP_CONTAINER_PATH, only_if_is_dirty=False)
        print(f"  Save result: {result}")
    except Exception as e:
        print(f"  Save error: {e}")

    print("\n" + "=" * 60)
    print("MIGRATION COMPLETE")
    print("=" * 60)
    return True


if __name__ == "__main__":
    migrate_container()
