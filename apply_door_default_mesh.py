"""
Apply DefaultDoorMesh to B_Door Blueprint CDO
"""
import unreal

bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Door"
door_mesh_path = "/Game/SoulslikeFramework/Meshes/SM/PrisonDoor/SM_PrisonDoor.SM_PrisonDoor"
door_frame_mesh_path = "/Game/SoulslikeFramework/Meshes/SM/PrisonDoor/SM_PrisonDoorArch.SM_PrisonDoorArch"

unreal.log("=" * 60)
unreal.log("Applying DefaultDoorMesh to B_Door Blueprint")
unreal.log("=" * 60)

# Load meshes
door_mesh = unreal.EditorAssetLibrary.load_asset(door_mesh_path)
door_frame_mesh = unreal.EditorAssetLibrary.load_asset(door_frame_mesh_path)

if not door_mesh:
    unreal.log_error(f"Could not load door mesh: {door_mesh_path}")
else:
    unreal.log(f"Loaded door mesh: {door_mesh.get_name()}")

if not door_frame_mesh:
    unreal.log_error(f"Could not load door frame mesh: {door_frame_mesh_path}")
else:
    unreal.log(f"Loaded door frame mesh: {door_frame_mesh.get_name()}")

# Load Blueprint
bp = unreal.EditorAssetLibrary.load_asset(bp_path)
if not bp:
    unreal.log_error(f"Could not load Blueprint: {bp_path}")
else:
    unreal.log(f"Loaded Blueprint: {bp.get_name()}")

    gen_class = bp.generated_class()
    if gen_class:
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            unreal.log("Setting DefaultDoorMesh and DefaultDoorFrameMesh...")

            # Set the TSoftObjectPtr properties
            try:
                cdo.set_editor_property('default_door_mesh', door_mesh)
                unreal.log("  Set default_door_mesh")
            except Exception as e:
                unreal.log_warning(f"  Could not set default_door_mesh: {e}")

            try:
                cdo.set_editor_property('default_door_frame_mesh', door_frame_mesh)
                unreal.log("  Set default_door_frame_mesh")
            except Exception as e:
                unreal.log_warning(f"  Could not set default_door_frame_mesh: {e}")

            # Save the Blueprint
            unreal.EditorAssetLibrary.save_asset(bp_path)
            unreal.log(f"Saved Blueprint: {bp_path}")

            # Verify
            verify_door = cdo.get_editor_property('default_door_mesh')
            verify_frame = cdo.get_editor_property('default_door_frame_mesh')
            unreal.log(f"Verification - DefaultDoorMesh: {verify_door}")
            unreal.log(f"Verification - DefaultDoorFrameMesh: {verify_frame}")

            if verify_door and verify_frame:
                unreal.log("SUCCESS! Door mesh properties set and saved.")
            else:
                unreal.log_warning("PARTIAL: One or both mesh properties not set correctly.")

unreal.log("=" * 60)
unreal.log("DONE")
unreal.log("=" * 60)
