# clear_container_eventgraph.py
# Clear B_Container's EventGraph but keep SCS components
# Similar approach to what was done for B_RestingPoint

import unreal

def clear_function_graphs(bp):
    """Clear all function graphs in the Blueprint"""
    cleared = 0
    try:
        function_graphs = bp.get_editor_property("function_graphs")
        if function_graphs:
            for fg in function_graphs:
                try:
                    fg_name = fg.get_name()
                    unreal.log_warning(f"  Found function graph: {fg_name}")
                except:
                    pass
    except Exception as e:
        unreal.log_warning(f"  function_graphs error: {e}")

def clear_uber_graphs(bp):
    """Clear uber graph (EventGraph)"""
    try:
        uber_graphs = bp.get_editor_property("uber_graph_pages")
        if uber_graphs:
            for ug in uber_graphs:
                try:
                    ug_name = ug.get_name()
                    unreal.log_warning(f"  Found uber graph: {ug_name}")
                    # Try to clear the graph
                    # unreal.BlueprintEditorLibrary.clear_uber_graph(bp, ug)
                except:
                    pass
    except Exception as e:
        unreal.log_warning(f"  uber_graphs error: {e}")

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("CLEARING B_Container EventGraph (KEEP_VARS_MAP pattern)")
    unreal.log_warning("=" * 70)

    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Container"
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_error(f"Could not load: {bp_path}")
        return

    unreal.log_warning(f"Loaded: {bp.get_name()}")

    # Use the SLFAutomationLibrary
    try:
        # First reparent to ASLFContainerBase (has interface)
        target_class = unreal.load_class(None, "/Script/SLFConversion.SLFContainerBase")
        if target_class:
            unreal.log_warning(f"Reparenting to: {target_class.get_name()}")
            unreal.BlueprintEditorLibrary.reparent_blueprint(bp, target_class)

        # Clear graphs but keep variables
        unreal.SLFAutomationLibrary.clear_graphs_keep_variables_no_compile(bp)
        unreal.log_warning("clear_graphs_keep_variables_no_compile completed")
    except Exception as e:
        unreal.log_error(f"Error: {e}")
        return

    # Compile
    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        unreal.log_warning("Compile completed")
    except Exception as e:
        unreal.log_warning(f"Compile: {e}")

    # Save
    try:
        unreal.EditorAssetLibrary.save_asset(bp_path)
        unreal.log_warning("Save completed")
    except Exception as e:
        unreal.log_error(f"Save: {e}")

    # Verify components
    bp2 = unreal.load_asset(bp_path)
    if bp2:
        gen_class = bp2.generated_class()
        if gen_class:
            world = unreal.EditorLevelLibrary.get_editor_world()
            if world:
                actor = unreal.EditorLevelLibrary.spawn_actor_from_class(gen_class, unreal.Vector(0, 0, -5000))
                if actor:
                    components = actor.get_components_by_class(unreal.ActorComponent)
                    unreal.log_warning(f"Spawned components: {len(components)}")
                    for comp in components:
                        unreal.log_warning(f"  - {comp.get_name()} ({type(comp).__name__})")
                    actor.destroy_actor()

    unreal.log_warning("=" * 70)

if __name__ == "__main__":
    main()
