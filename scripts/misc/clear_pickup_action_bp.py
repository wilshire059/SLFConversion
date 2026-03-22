import unreal

# Clear the Blueprint's EventGraph so C++ implementation runs
bp_path = "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action_PickupItemMontage"

bp = unreal.EditorAssetLibrary.load_asset(bp_path)

if bp:
    print(f"Found Blueprint: {bp.get_name()}")
    print(f"Class: {bp.get_class().get_name()}")

    # Get the Blueprint's graphs and clear them
    try:
        # Use the BlueprintEditorLibrary to access graphs
        editor_subsystem = unreal.get_editor_subsystem(unreal.AssetEditorSubsystem)

        # Try to clear the EventGraph using the editor library
        blueprint_library = unreal.BlueprintEditorLibrary

        # Get all graphs
        all_graphs = bp.get_editor_property('function_graphs') if hasattr(bp, 'get_editor_property') else []
        ubergraph = bp.get_editor_property('uber_graph_pages') if hasattr(bp, 'get_editor_property') else []

        print(f"Function graphs: {all_graphs}")
        print(f"Uber graph pages: {ubergraph}")

        # Try clearing the event graph via another approach
        # Mark the Blueprint as needing recompile
        if hasattr(unreal, 'KismetEditorUtilities'):
            unreal.KismetEditorUtilities.compile_blueprint(bp)
            print("Compiled Blueprint")

        # Remove all nodes from event graph
        # This requires the Blueprint editor API
        print("\nAttempting to clear EventGraph nodes...")

        # Try using the FKismetEditorUtilities approach
        result = unreal.BlueprintEditorLibrary.clear_all_blueprint_functions(bp)
        print(f"clear_all_blueprint_functions result: {result}")

        # Try the Kismet editor utilities
        if hasattr(unreal, 'KismetEditorUtilities'):
            try:
                # Get the event graph
                graphs = unreal.BlueprintEditorLibrary.get_all_graphs_from_blueprint(bp)
                print(f"All graphs: {graphs}")
                for graph in graphs:
                    print(f"  Graph: {graph.get_name()}")
            except Exception as e:
                print(f"Error getting graphs: {e}")

        # Compile again
        unreal.KismetEditorUtilities.compile_blueprint(bp)
        print("Recompiled Blueprint")

        # Save
        unreal.EditorAssetLibrary.save_asset(bp_path)
        print("Saved Blueprint")

    except Exception as e:
        print(f"Error: {e}")
        import traceback
        traceback.print_exc()
else:
    print(f"Could not load: {bp_path}")
