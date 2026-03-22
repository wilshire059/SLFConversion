# check_character_mesh.py
# Check character mesh component hierarchy

import unreal
import json

def log(msg):
    print(f"[Char] {msg}")
    unreal.log_warning(f"[Char] {msg}")

CHARACTER_PATH = "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character"

def main():
    log("=" * 70)
    log("CHECKING CHARACTER MESH COMPONENT HIERARCHY")
    log("=" * 70)

    bp = unreal.load_asset(CHARACTER_PATH)
    if not bp:
        log(f"Could not load character")
        return

    gen_class = bp.generated_class()
    if not gen_class:
        log(f"No generated class")
        return

    # Spawn character
    try:
        world = unreal.EditorLevelLibrary.get_editor_world()
        if not world:
            log(f"No world")
            return

        spawned = unreal.EditorLevelLibrary.spawn_actor_from_class(
            gen_class,
            unreal.Vector(0, 0, 500),
            unreal.Rotator(0, 0, 0)
        )

        if not spawned:
            log(f"Failed to spawn")
            return

        log(f"Spawned: {spawned.get_name()}")

        # List all skeletal mesh components
        log(f"\nAll SkeletalMeshComponents:")
        mesh_comps = spawned.get_components_by_class(unreal.SkeletalMeshComponent)
        for i, comp in enumerate(mesh_comps):
            comp_name = comp.get_name()
            skel_mesh = comp.get_editor_property('skeletal_mesh_asset')
            mesh_name = skel_mesh.get_name() if skel_mesh else "NULL"
            log(f"  {i}: {comp_name} - Mesh: {mesh_name}")

            # Check parent
            parent = comp.get_attach_parent()
            parent_name = parent.get_name() if parent else "ROOT"
            log(f"     Parent: {parent_name}")

            # Get relative transform
            rel_loc = comp.get_editor_property('relative_location')
            rel_rot = comp.get_editor_property('relative_rotation')
            log(f"     RelLoc: X={rel_loc.x:.4f}, Y={rel_loc.y:.4f}, Z={rel_loc.z:.4f}")
            log(f"     RelRot: P={rel_rot.pitch:.4f}, Y={rel_rot.yaw:.4f}, R={rel_rot.roll:.4f}")

            # Check for hand_r socket
            if comp.does_socket_exist('hand_r'):
                log(f"     Socket 'hand_r' EXISTS")
                socket_transform = comp.get_socket_transform('hand_r', unreal.RelativeTransformSpace.RTS_PARENT_BONE_SPACE)
                log(f"       Location: {socket_transform.translation}")
                log(f"       Rotation: {socket_transform.rotation.rotator()}")
            else:
                log(f"     Socket 'hand_r' NOT found")

        spawned.destroy_actor()

    except Exception as e:
        log(f"Error: {e}")
        import traceback
        log(traceback.format_exc())

if __name__ == "__main__":
    main()
