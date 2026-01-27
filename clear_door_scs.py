"""
Clear B_Door's SCS to force it to use C++ components
"""
import unreal

def clear_door_scs():
    """Clear B_Door Blueprint's SCS and add Interactable SM component"""

    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Door"
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)

    if not bp:
        print(f"ERROR: Could not load {bp_path}")
        return False

    print(f"Loaded Blueprint: {bp.get_name()}")
    print(f"Class: {bp.get_class().get_name()}")

    # Try to access SCS via get_editor_property
    try:
        scs = bp.get_editor_property('simple_construction_script')
        if scs:
            print(f"Found SCS via get_editor_property")

            # Get default scene root node
            default_root = scs.get_editor_property('default_scene_root_node')
            if default_root:
                print(f"  Default root node: {default_root}")

            # Get all nodes
            all_nodes = scs.get_editor_property('all_nodes')
            if all_nodes:
                print(f"  All nodes: {len(all_nodes)}")
                for node in all_nodes:
                    var_name = node.get_editor_property('internal_variable_name') if hasattr(node, 'get_editor_property') else "?"
                    print(f"    - {var_name}")
        else:
            print("SCS is None")
    except Exception as e:
        print(f"Could not get SCS via get_editor_property: {e}")

    # Alternative: Try BlueprintEditorLibrary
    try:
        from unreal import BlueprintEditorLibrary
        # Compile the blueprint
        print("\nCompiling Blueprint...")
        BlueprintEditorLibrary.compile_blueprint(bp)
        print("Compiled")
    except Exception as e:
        print(f"BlueprintEditorLibrary error: {e}")

    # Check generated class and CDO components
    gen_class = bp.generated_class()
    if gen_class:
        print(f"\nGenerated class: {gen_class.get_name()}")

        cdo = unreal.get_default_object(gen_class)
        if cdo:
            print(f"CDO: {cdo.get_name()}")

            # List all static mesh components
            sm_comps = cdo.get_components_by_class(unreal.StaticMeshComponent)
            print(f"\nStaticMeshComponents in CDO ({len(sm_comps)}):")
            for comp in sm_comps:
                mesh_name = comp.static_mesh.get_name() if comp.static_mesh else "NO MESH"
                print(f"  - {comp.get_name()}: {mesh_name}")

            # Check if we have a DoorMesh component from C++
            found_door_mesh = False
            for comp in sm_comps:
                if "DoorMesh" in comp.get_name():
                    found_door_mesh = True
                    print(f"\n  Found C++ DoorMesh component!")
                    break

            if not found_door_mesh:
                print("\n  WARNING: No C++ DoorMesh component found!")
                print("  The Blueprint SCS is overriding C++ components.")

    # Save
    print("\nSaving Blueprint...")
    result = unreal.EditorAssetLibrary.save_asset(bp_path)
    print(f"Save result: {result}")

    return True

# Run
clear_door_scs()
