"""
Apply door mesh to B_Door Blueprint CDO
"""
import unreal

bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Door"
door_mesh_path = "/Game/SoulslikeFramework/Meshes/SM/PrisonDoor/SM_PrisonDoor.SM_PrisonDoor"

unreal.log(f"Loading door mesh: {door_mesh_path}")
door_mesh = unreal.EditorAssetLibrary.load_asset(door_mesh_path)
if not door_mesh:
    unreal.log_error(f"Could not load door mesh: {door_mesh_path}")
else:
    unreal.log(f"Loaded mesh: {door_mesh.get_name()}")

    unreal.log(f"Loading Blueprint: {bp_path}")
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        unreal.log_error(f"Could not load Blueprint: {bp_path}")
    else:
        gen_class = bp.generated_class()
        if gen_class:
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                # Find Interactable SM component
                all_comps = cdo.get_components_by_class(unreal.StaticMeshComponent)
                for comp in all_comps:
                    if comp.get_name() == "Interactable SM":
                        current_mesh = comp.static_mesh
                        unreal.log(f"Found 'Interactable SM', current mesh: {current_mesh.get_name() if current_mesh else 'None'}")

                        # Set the mesh
                        comp.set_static_mesh(door_mesh)
                        new_mesh = comp.static_mesh
                        unreal.log(f"Set mesh to: {new_mesh.get_name() if new_mesh else 'None'}")

                        # Save the Blueprint
                        unreal.EditorAssetLibrary.save_asset(bp_path)
                        unreal.log(f"Saved Blueprint: {bp_path}")

                        # Verify
                        verify_mesh = comp.static_mesh
                        if verify_mesh:
                            unreal.log(f"SUCCESS: Door mesh set to {verify_mesh.get_name()}")
                        else:
                            unreal.log_error("FAILED: Mesh is still None after save")
                        break
                else:
                    unreal.log_error("Could not find 'Interactable SM' component")
