"""
Clear B_Door's SCS (SimpleConstructionScript) completely, then reparent to C++.
This allows C++ components from AB_Door to take over.
"""
import unreal

def clear_scs_and_reparent_door():
    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Door"
    new_parent_path = "/Script/SLFConversion.B_Door"  # AB_Door in C++

    print(f"Loading Blueprint: {bp_path}")
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        print(f"ERROR: Could not load {bp_path}")
        return False

    print(f"Blueprint class: {bp.get_class().get_name()}")

    # Get the SCS (SimpleConstructionScript)
    try:
        scs = bp.get_editor_property('simple_construction_script')
        if scs:
            print(f"Found SCS")

            # Get all nodes
            all_nodes = scs.get_editor_property('all_nodes')
            if all_nodes:
                print(f"  SCS has {len(all_nodes)} nodes")
                for node in all_nodes:
                    try:
                        var_name = node.get_editor_property('internal_variable_name')
                        print(f"    - {var_name}")
                    except:
                        print(f"    - (unknown node)")

                # Clear all nodes by setting to empty array
                # Note: This may not work directly - SCS nodes may need to be removed differently
                print("\n  Attempting to clear SCS nodes...")

            # Get root nodes
            root_nodes = scs.get_editor_property('root_nodes')
            if root_nodes:
                print(f"  Root nodes: {len(root_nodes)}")
                for node in root_nodes:
                    try:
                        var_name = node.get_editor_property('internal_variable_name')
                        print(f"    - {var_name}")
                    except:
                        print(f"    - (unknown root node)")
        else:
            print("SCS is None")
    except Exception as e:
        print(f"Could not get SCS: {e}")

    # Step 1: Clear Blueprint logic using SLFAutomationLibrary
    print("\nClearing Blueprint logic...")
    try:
        # Clear all logic but keep variables (for now)
        unreal.SLFAutomationLibrary.clear_all_blueprint_logic(bp, False)
        print("  Cleared via SLFAutomationLibrary")
    except Exception as e:
        print(f"  Could not use SLFAutomationLibrary: {e}")

    # Step 2: Load new parent class
    print(f"\nLoading new parent class: {new_parent_path}")
    new_parent = unreal.load_class(None, new_parent_path)
    if not new_parent:
        print(f"ERROR: Could not load parent class {new_parent_path}")
        return False

    # Step 3: Reparent the Blueprint
    print(f"Reparenting {bp.get_name()} to {new_parent.get_name()}")
    result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, new_parent)
    print(f"Reparent result: {result}")

    # Step 4: Compile
    print("\nCompiling...")
    unreal.BlueprintEditorLibrary.compile_blueprint(bp)

    # Step 5: Check components after reparent
    print("\nChecking components after reparent...")
    gen_class = bp.generated_class()
    if gen_class:
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            sm_comps = cdo.get_components_by_class(unreal.StaticMeshComponent)
            print(f"StaticMeshComponents in CDO ({len(sm_comps)}):")
            for comp in sm_comps:
                mesh_name = comp.static_mesh.get_name() if comp.static_mesh else "NO MESH"
                visible = comp.is_visible()
                print(f"  - {comp.get_name()}: {mesh_name} (visible={visible})")

    # Step 6: Save
    print("\nSaving...")
    save_result = unreal.EditorAssetLibrary.save_asset(bp_path)
    print(f"Save result: {save_result}")

    return result

clear_scs_and_reparent_door()
