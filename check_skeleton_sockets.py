# check_skeleton_sockets.py
# Check sockets directly on skeleton by loading the skeletal mesh

import unreal
import json

def log(msg):
    print(f"[Socket] {msg}")
    unreal.log_warning(f"[Socket] {msg}")

# Skeletal mesh paths to check
MESH_PATHS = [
    "/Game/SoulslikeFramework/Demo/SKM/Mannequins/Meshes/SKM_Manny",
    "/Game/SoulslikeFramework/Demo/SKM/Mannequins/Meshes/SKM_Quinn",
]

def main():
    log("=" * 70)
    log("CHECKING SKELETON SOCKETS DIRECTLY")
    log("=" * 70)

    results = {}

    for mesh_path in MESH_PATHS:
        log(f"\n--- SkeletalMesh: {mesh_path} ---")

        skel_mesh = unreal.load_asset(mesh_path)
        if not skel_mesh:
            log(f"  Could not load skeletal mesh")
            continue

        log(f"  Loaded: {skel_mesh.get_name()}")

        # Get skeleton
        skeleton = skel_mesh.get_editor_property('skeleton')
        if not skeleton:
            log(f"  No skeleton")
            continue

        skel_name = skeleton.get_name()
        log(f"  Skeleton: {skel_name}")

        mesh_data = {"skeleton": skel_name, "sockets": []}

        # Try to get socket names from the skeletal mesh's post-process anim inst class
        # Actually, we need to check if there's a virtual bone or socket
        # The sockets are on the skeleton, not directly accessible via Python

        # Let's spawn a temp actor with this skeletal mesh and check sockets
        try:
            world = unreal.EditorLevelLibrary.get_editor_world()
            if not world:
                log(f"  No world")
                continue

            # Create a temp skeletal mesh actor
            temp_actor = unreal.EditorLevelLibrary.spawn_actor_from_class(
                unreal.SkeletalMeshActor,
                unreal.Vector(0, 0, 2000),
                unreal.Rotator(0, 0, 0)
            )
            if not temp_actor:
                log(f"  Failed to spawn temp actor")
                continue

            # Get the skeletal mesh component and set the mesh
            mesh_comp = temp_actor.get_editor_property('skeletal_mesh_component')
            if not mesh_comp:
                log(f"  No mesh component on temp actor")
                temp_actor.destroy_actor()
                continue

            # Set the skeletal mesh
            mesh_comp.set_skeletal_mesh_asset(skel_mesh)

            log(f"  Set mesh on temp actor")

            # Now check sockets
            socket_names_to_check = ['hand_r', 'hand_l', 'weapon_r', 'weapon_l', 'YOURHAND',
                                      'hand_rSocket', 'Hand_R', 'HandR', 'RightHand', 'right_hand']

            for socket_name in socket_names_to_check:
                if mesh_comp.does_socket_exist(socket_name):
                    socket_transform = mesh_comp.get_socket_transform(socket_name, unreal.RelativeTransformSpace.RTS_PARENT_BONE_SPACE)
                    log(f"  Socket: {socket_name} EXISTS")
                    log(f"    Location: {socket_transform.translation}")
                    log(f"    Rotation: {socket_transform.rotation.rotator()}")

                    mesh_data["sockets"].append({
                        "name": socket_name,
                        "location": {
                            "x": socket_transform.translation.x,
                            "y": socket_transform.translation.y,
                            "z": socket_transform.translation.z
                        },
                        "rotation": {
                            "pitch": socket_transform.rotation.rotator().pitch,
                            "yaw": socket_transform.rotation.rotator().yaw,
                            "roll": socket_transform.rotation.rotator().roll
                        }
                    })

            # Also try some bones as sockets (bones can be used as attachment points)
            bone_names = ['hand_r', 'hand_l', 'lowerarm_r', 'lowerarm_l']
            for bone_name in bone_names:
                bone_idx = mesh_comp.get_bone_index(bone_name)
                if bone_idx >= 0:
                    log(f"  Bone: {bone_name} exists (index {bone_idx})")

            temp_actor.destroy_actor()

        except Exception as e:
            log(f"  Error: {e}")
            import traceback
            log(traceback.format_exc())

        results[mesh_path] = mesh_data

    # Save results
    output_path = "C:/scripts/SLFConversion/migration_cache/skeleton_socket_check.json"
    with open(output_path, 'w') as f:
        json.dump(results, f, indent=2)
    log(f"\nSaved to: {output_path}")

if __name__ == "__main__":
    main()
