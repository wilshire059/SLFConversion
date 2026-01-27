"""
Clear B_SkyManager Blueprint components/graphs and reparent to SLFSkyManager C++ class
The C++ class will provide all components via CreateDefaultSubobject
"""
import unreal

BP_PATH = "/Game/SoulslikeFramework/Blueprints/Sky/B_SkyManager"
CPP_CLASS = "/Script/SLFConversion.SLFSkyManager"

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("CLEAR AND REPARENT B_SkyManager")
    unreal.log_warning("=" * 70)

    # Load Blueprint
    bp = unreal.EditorAssetLibrary.load_asset(BP_PATH)
    if not bp:
        unreal.log_warning(f"ERROR: Could not load Blueprint: {BP_PATH}")
        return

    unreal.log_warning(f"Loaded Blueprint: {bp.get_name()}")

    # Step 1: Clear all variables (they will come from C++ parent)
    unreal.log_warning("\nStep 1: Removing Blueprint variables...")
    try:
        variables = unreal.BlueprintEditorLibrary.get_blueprint_variables(bp)
        for var in variables:
            var_name = var.get_editor_property('member_name')
            unreal.log_warning(f"  Removing variable: {var_name}")
            try:
                unreal.BlueprintEditorLibrary.remove_blueprint_variable(bp, var_name)
            except Exception as e:
                unreal.log_warning(f"    Error removing {var_name}: {e}")
    except Exception as e:
        unreal.log_warning(f"  Error listing variables: {e}")

    # Step 2: Clear EventGraph
    unreal.log_warning("\nStep 2: Clearing EventGraph...")
    try:
        graphs = bp.ubergraph_pages
        for graph in graphs:
            unreal.log_warning(f"  Clearing graph: {graph.get_name()}")
            nodes = graph.nodes
            for node in list(nodes):
                try:
                    graph.remove_node(node)
                except:
                    pass
    except Exception as e:
        unreal.log_warning(f"  Error clearing graphs: {e}")

    # Step 3: Clear function graphs
    unreal.log_warning("\nStep 3: Clearing function graphs...")
    try:
        function_graphs = bp.function_graphs
        for fg in list(function_graphs):
            unreal.log_warning(f"  Found function: {fg.get_name()}")
            # Don't remove built-in graphs, just clear them
            nodes = fg.nodes
            for node in list(nodes):
                try:
                    fg.remove_node(node)
                except:
                    pass
    except Exception as e:
        unreal.log_warning(f"  Error with function graphs: {e}")

    # Step 4: Save the cleared Blueprint first
    unreal.log_warning("\nStep 4: Saving cleared Blueprint...")
    try:
        unreal.EditorAssetLibrary.save_asset(BP_PATH)
        unreal.log_warning("  Saved")
    except Exception as e:
        unreal.log_warning(f"  Save error: {e}")

    # Step 5: Reload and reparent
    unreal.log_warning("\nStep 5: Reloading and reparenting...")
    bp = unreal.EditorAssetLibrary.load_asset(BP_PATH)

    cpp_class = unreal.load_class(None, CPP_CLASS)
    if not cpp_class:
        unreal.log_warning(f"ERROR: Could not load C++ class: {CPP_CLASS}")
        return

    unreal.log_warning(f"  Target C++ class: {cpp_class.get_name()}")

    try:
        result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, cpp_class)
        unreal.log_warning(f"  Reparent result: {result}")
    except Exception as e:
        unreal.log_warning(f"  Reparent error: {e}")
        return

    # Step 6: Compile
    unreal.log_warning("\nStep 6: Compiling...")
    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        unreal.log_warning("  Compiled")
    except Exception as e:
        unreal.log_warning(f"  Compile error: {e}")

    # Step 7: Save again
    unreal.log_warning("\nStep 7: Final save...")
    try:
        unreal.EditorAssetLibrary.save_asset(BP_PATH)
        unreal.log_warning("  Saved")
    except Exception as e:
        unreal.log_warning(f"  Save error: {e}")

    # Verify
    unreal.log_warning("\nVerifying...")
    bp = unreal.EditorAssetLibrary.load_asset(BP_PATH)
    gen_class = bp.generated_class()
    if gen_class:
        is_child = unreal.MathLibrary.class_is_child_of(gen_class, cpp_class)
        unreal.log_warning(f"  Is child of SLFSkyManager: {is_child}")

    unreal.log_warning("\n" + "=" * 70)
    unreal.log_warning("DONE")
    unreal.log_warning("=" * 70)

main()
