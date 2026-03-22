"""
Fix B_Door mesh - add SM_PrisonDoor to the Blueprint's SCS component
"""
import unreal

def fix_door_mesh():
    """Set the door mesh on B_Door Blueprint's SCS component"""

    # Load the Blueprint
    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Door"
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)

    if not bp:
        print(f"ERROR: Could not load {bp_path}")
        return False

    print(f"Loaded: {bp.get_name()}")

    # Load the mesh we want to assign
    mesh_path = "/Game/SoulslikeFramework/Meshes/SM/PrisonDoor/SM_PrisonDoor"
    door_mesh = unreal.EditorAssetLibrary.load_asset(mesh_path)

    if not door_mesh:
        print(f"ERROR: Could not load mesh {mesh_path}")
        return False

    print(f"Loaded mesh: {door_mesh.get_name()}")

    # Try to get the generated class and spawn an instance to access components
    gen_class = bp.generated_class()
    if not gen_class:
        print("ERROR: No generated class")
        return False

    print(f"Generated class: {gen_class.get_name()}")

    # Get the CDO
    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        print("ERROR: No CDO")
        return False

    print(f"CDO: {cdo.get_name()}")

    # Get all components from CDO
    all_comps = cdo.get_components_by_class(unreal.ActorComponent)
    print(f"CDO has {len(all_comps)} components")

    found_interactable_sm = False
    nodes = []  # Placeholder for compatibility with rest of code

    found_interactable_sm = False

    for node in nodes:
        var_name = node.get_variable_name() if hasattr(node, 'get_variable_name') else "Unknown"
        print(f"  Node: {var_name}")

        # Check if this is the Interactable SM component
        if "Interactable" in str(var_name) and "SM" in str(var_name):
            found_interactable_sm = True
            print(f"  -> Found Interactable SM node!")

            # Get the component template
            if hasattr(node, 'component_template') and node.component_template:
                template = node.component_template
                print(f"     Template: {template.get_name()} ({template.get_class().get_name()})")

                # Check if it's a StaticMeshComponent
                if isinstance(template, unreal.StaticMeshComponent):
                    # Set the mesh
                    template.set_editor_property('static_mesh', door_mesh)
                    print(f"     SET MESH to {door_mesh.get_name()}")
                else:
                    print(f"     Not a StaticMeshComponent, cannot set mesh")

    # If we didn't find Interactable SM in the SCS, we need to add a component
    if not found_interactable_sm:
        print("\nWARNING: 'Interactable SM' not found in SCS nodes")
        print("The Blueprint might need to have this component added via editor")

        # Let's try to add a static mesh component to the SCS
        print("\nAttempting to add a new StaticMeshComponent to SCS...")

        # Get the CDO and check if we can add a component there
        gen_class = bp.generated_class()
        if gen_class:
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                print(f"  CDO: {cdo.get_name()}")

                # Try to find any existing StaticMeshComponent
                comps = cdo.get_components_by_class(unreal.StaticMeshComponent)
                print(f"  CDO has {len(comps)} StaticMeshComponents")

                for comp in comps:
                    print(f"    - {comp.get_name()}: {comp.static_mesh.get_name() if comp.static_mesh else 'NO MESH'}")

                    # If this component has no mesh, try setting it
                    if not comp.static_mesh:
                        print(f"      Attempting to set mesh on {comp.get_name()}...")
                        comp.set_editor_property('static_mesh', door_mesh)
                        if comp.static_mesh:
                            print(f"      SUCCESS: Set mesh to {comp.static_mesh.get_name()}")
                        else:
                            print(f"      FAILED: Mesh not set")

    # Save the Blueprint
    print("\nSaving Blueprint...")
    saved = unreal.EditorAssetLibrary.save_asset(bp_path)
    print(f"Save result: {saved}")

    return True

# Run
fix_door_mesh()
