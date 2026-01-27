"""
Clear B_Door's Blueprint logic and SCS, then reparent to C++
"""
import unreal

def clear_and_reparent_door():
    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Door"
    new_parent_path = "/Script/SLFConversion.B_Door"  # AB_Door in C++

    print(f"Loading Blueprint: {bp_path}")
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        print(f"ERROR: Could not load {bp_path}")
        return False

    print(f"Loading new parent class: {new_parent_path}")
    new_parent = unreal.load_class(None, new_parent_path)
    if not new_parent:
        print(f"ERROR: Could not load parent class {new_parent_path}")
        return False

    # Step 1: Clear all Blueprint logic (event graph, functions)
    print("Clearing Blueprint logic...")
    try:
        # Use SLFAutomationLibrary if available
        unreal.SLFAutomationLibrary.clear_all_blueprint_logic(bp, False)  # False = don't keep variables
        print("  Cleared via SLFAutomationLibrary")
    except Exception as e:
        print(f"  Could not use SLFAutomationLibrary: {e}")
        # Fallback: try to clear manually
        try:
            # Get function graphs and clear them
            function_graphs = bp.get_editor_property('function_graphs')
            if function_graphs:
                print(f"  Found {len(function_graphs)} function graphs")
            # Clear UbergraphPages (event graph)
            ubergraph_pages = bp.get_editor_property('ubergraph_pages')
            if ubergraph_pages:
                print(f"  Found {len(ubergraph_pages)} ubergraph pages")
        except Exception as e2:
            print(f"  Could not access graph properties: {e2}")

    # Step 2: Reparent the Blueprint
    print(f"Reparenting {bp.get_name()} to {new_parent.get_name()}")
    result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, new_parent)
    print(f"Reparent result: {result}")

    # Step 3: Compile
    print("Compiling...")
    unreal.BlueprintEditorLibrary.compile_blueprint(bp)

    # Step 4: Save
    print("Saving...")
    save_result = unreal.EditorAssetLibrary.save_asset(bp_path)
    print(f"Save result: {save_result}")

    return result

clear_and_reparent_door()
