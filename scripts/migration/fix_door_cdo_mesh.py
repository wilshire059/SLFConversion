"""
Directly set the mesh on B_Door CDO components.
This is the correct fix - set the mesh directly on the component in the Blueprint CDO.
"""
import unreal

bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Door"
door_mesh_path = "/Game/SoulslikeFramework/Meshes/SM/PrisonDoor/SM_PrisonDoor.SM_PrisonDoor"
frame_mesh_path = "/Game/SoulslikeFramework/Meshes/SM/PrisonDoor/SM_PrisonDoorArch.SM_PrisonDoorArch"

unreal.log("=" * 70)
unreal.log("FIXING B_Door CDO Component Meshes")
unreal.log("=" * 70)

# Load meshes
door_mesh = unreal.EditorAssetLibrary.load_asset(door_mesh_path)
frame_mesh = unreal.EditorAssetLibrary.load_asset(frame_mesh_path)

if not door_mesh:
    unreal.log_error(f"Could not load door mesh: {door_mesh_path}")
else:
    unreal.log(f"Loaded door mesh: {door_mesh.get_name()}")

if not frame_mesh:
    unreal.log_error(f"Could not load frame mesh: {frame_mesh_path}")
else:
    unreal.log(f"Loaded frame mesh: {frame_mesh.get_name()}")

# Load Blueprint
bp = unreal.EditorAssetLibrary.load_asset(bp_path)
if not bp:
    unreal.log_error(f"Could not load Blueprint: {bp_path}")
else:
    unreal.log(f"Loaded Blueprint: {bp.get_name()}")

    gen_class = bp.generated_class()
    if not gen_class:
        unreal.log_error("No generated class")
    else:
        cdo = unreal.get_default_object(gen_class)
        if not cdo:
            unreal.log_error("No CDO")
        else:
            # Get all StaticMeshComponents
            mesh_comps = cdo.get_components_by_class(unreal.StaticMeshComponent)
            unreal.log(f"\nFound {len(mesh_comps)} StaticMeshComponents")

            for comp in mesh_comps:
                comp_name = comp.get_name()
                current_mesh = comp.static_mesh
                unreal.log(f"\n  Component: {comp_name}")
                unreal.log(f"    Current mesh: {current_mesh.get_name() if current_mesh else 'NO_MESH'}")

                # Set door mesh on "Interactable SM"
                if "Interactable" in comp_name:
                    if door_mesh:
                        comp.set_static_mesh(door_mesh)
                        unreal.log(f"    -> Set mesh to: {door_mesh.get_name()}")

                # Door Frame should already have mesh, but set it just in case
                elif "Door Frame" in comp_name or "Frame" in comp_name:
                    if frame_mesh:
                        comp.set_static_mesh(frame_mesh)
                        unreal.log(f"    -> Set mesh to: {frame_mesh.get_name()}")

            # Mark Blueprint as modified
            unreal.EditorAssetLibrary.save_asset(bp_path, only_if_is_dirty=False)
            unreal.log(f"\nSaved Blueprint: {bp_path}")

            # Verify
            unreal.log("\n" + "=" * 70)
            unreal.log("VERIFICATION")
            unreal.log("=" * 70)

            mesh_comps2 = cdo.get_components_by_class(unreal.StaticMeshComponent)
            for comp in mesh_comps2:
                mesh = comp.static_mesh
                unreal.log(f"  {comp.get_name()}: {mesh.get_name() if mesh else 'NO_MESH'}")

unreal.log("\n" + "=" * 70)
unreal.log("DONE")
unreal.log("=" * 70)
