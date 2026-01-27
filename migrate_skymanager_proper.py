"""
Proper two-pass clear-and-reparent for B_SkyManager
Based on successful patterns from boss_door and interactables migrations

This script:
1. Clears EventGraph nodes (removes problematic Blueprint logic)
2. SAVES the cleared state (critical!)
3. Reloads and reparents to C++ class
4. Compiles and saves again

SCS components (DirectionalLight_Sun, DirectionalLight_Moon, etc.) are PRESERVED
"""
import unreal

BP_PATH = "/Game/SoulslikeFramework/Blueprints/Sky/B_SkyManager"
CPP_CLASS = "/Script/SLFConversion.SLFSkyManager"

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("PROPER TWO-PASS SKYMANAGER MIGRATION")
    unreal.log_warning("=" * 70)

    # ========================================
    # PHASE 1: Load and clear graphs
    # ========================================
    unreal.log_warning("\n[PHASE 1] Loading and clearing graphs...")

    bp = unreal.EditorAssetLibrary.load_asset(BP_PATH)
    if not bp:
        unreal.log_warning(f"ERROR: Could not load {BP_PATH}")
        return

    unreal.log_warning(f"  Loaded: {bp.get_name()}")

    # Get current parent
    gen_class = bp.generated_class()
    if gen_class:
        try:
            parent = unreal.get_type_from_class(gen_class)
            unreal.log_warning(f"  Current parent class: {gen_class.get_name()}")
        except:
            unreal.log_warning(f"  Generated class: {gen_class.get_name()}")

    # Clear UberGraphPages (EventGraph)
    try:
        graphs = bp.get_editor_property('uber_graph_pages')
        if graphs:
            unreal.log_warning(f"  Found {len(graphs)} uber graph pages")
            for graph in graphs:
                nodes = graph.get_editor_property('nodes')
                if nodes:
                    node_count = len(nodes)
                    nodes.clear()
                    unreal.log_warning(f"    Cleared {node_count} nodes from {graph.get_name()}")
    except Exception as e:
        unreal.log_warning(f"  uber_graph_pages error: {e}")

    # Clear function graphs
    try:
        func_graphs = bp.get_editor_property('function_graphs')
        if func_graphs:
            unreal.log_warning(f"  Found {len(func_graphs)} function graphs")
            for fg in func_graphs:
                nodes = fg.get_editor_property('nodes')
                if nodes:
                    node_count = len(nodes)
                    nodes.clear()
                    unreal.log_warning(f"    Cleared {node_count} nodes from {fg.get_name()}")
    except Exception as e:
        unreal.log_warning(f"  function_graphs error: {e}")

    # ========================================
    # PHASE 2: SAVE the cleared state (CRITICAL!)
    # ========================================
    unreal.log_warning("\n[PHASE 2] Saving cleared state...")
    try:
        result = unreal.EditorAssetLibrary.save_asset(BP_PATH)
        unreal.log_warning(f"  Save result: {result}")
    except Exception as e:
        unreal.log_warning(f"  Save error: {e}")
        return

    # ========================================
    # PHASE 3: Reload and reparent
    # ========================================
    unreal.log_warning("\n[PHASE 3] Reloading and reparenting...")

    # Reload the cleared Blueprint
    bp = unreal.EditorAssetLibrary.load_asset(BP_PATH)
    if not bp:
        unreal.log_warning("  ERROR: Could not reload Blueprint")
        return

    # Load target C++ class
    cpp_class = unreal.load_class(None, CPP_CLASS)
    if not cpp_class:
        unreal.log_warning(f"  ERROR: Could not load C++ class: {CPP_CLASS}")
        return

    unreal.log_warning(f"  Target C++ class: {cpp_class.get_name()}")

    # Reparent
    try:
        result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, cpp_class)
        unreal.log_warning(f"  Reparent result: {result}")
    except Exception as e:
        unreal.log_warning(f"  Reparent error: {e}")
        return

    # ========================================
    # PHASE 4: Compile and save
    # ========================================
    unreal.log_warning("\n[PHASE 4] Compiling and saving...")

    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        unreal.log_warning("  Compiled")
    except Exception as e:
        unreal.log_warning(f"  Compile error: {e}")

    try:
        result = unreal.EditorAssetLibrary.save_asset(BP_PATH)
        unreal.log_warning(f"  Save result: {result}")
    except Exception as e:
        unreal.log_warning(f"  Save error: {e}")

    # ========================================
    # PHASE 5: Verify
    # ========================================
    unreal.log_warning("\n[PHASE 5] Verifying...")

    bp = unreal.EditorAssetLibrary.load_asset(BP_PATH)
    gen_class = bp.generated_class()
    if gen_class:
        unreal.log_warning(f"  Generated class: {gen_class.get_name()}")

        is_child = unreal.MathLibrary.class_is_child_of(gen_class, cpp_class)
        unreal.log_warning(f"  Is child of SLFSkyManager: {is_child}")

    unreal.log_warning("\n" + "=" * 70)
    unreal.log_warning("DONE - SCS components should be preserved")
    unreal.log_warning("=" * 70)

main()
