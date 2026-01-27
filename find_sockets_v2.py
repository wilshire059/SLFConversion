# find_sockets_v2.py
# Find sockets on character skeletal meshes

import unreal
import json

def log(msg):
    print(f"[Socket] {msg}")
    unreal.log_warning(f"[Socket] {msg}")

def main():
    log("=" * 70)
    log("FINDING CHARACTER SKELETAL MESH SOCKETS")
    log("=" * 70)

    results = {}

    # Try loading known skeletal mesh paths
    mesh_paths = [
        "/Game/Characters/Mannequins/Meshes/SKM_Manny",
        "/Game/Characters/Mannequins/Meshes/SKM_Quinn",
        "/Game/SoulslikeFramework/Demo/_Character/Meshes/SKM_Manny",
        "/Game/SoulslikeFramework/Demo/_Character/Meshes/SKM_Quinn",
    ]

    for mesh_path in mesh_paths:
        log(f"\n--- Trying: {mesh_path} ---")

        skel_mesh = unreal.load_asset(mesh_path)
        if not skel_mesh:
            log(f"  Not found")
            continue

        log(f"  Loaded: {skel_mesh.get_name()}")

        # Get skeleton from skeletal mesh
        skeleton = skel_mesh.get_editor_property('skeleton')
        if not skeleton:
            log(f"  No skeleton property")
            continue

        skel_name = skeleton.get_name()
        log(f"  Skeleton: {skel_name}")

        socket_count = skeleton.num_sockets()
        log(f"  Socket count: {socket_count}")

        hand_sockets = []
        for i in range(socket_count):
            try:
                socket = skeleton.get_socket_by_index(i)
                if socket:
                    socket_name = socket.get_name()
                    rel_loc = socket.get_editor_property('relative_location')
                    rel_rot = socket.get_editor_property('relative_rotation')
                    bone_name = socket.get_editor_property('bone_name')

                    # Only show hand/weapon related
                    if 'hand' in socket_name.lower() or 'weapon' in socket_name.lower():
                        log(f"    Socket: {socket_name}")
                        log(f"      Bone: {bone_name}")
                        log(f"      RelLoc: X={rel_loc.x:.4f}, Y={rel_loc.y:.4f}, Z={rel_loc.z:.4f}")
                        log(f"      RelRot: P={rel_rot.pitch:.2f}, Y={rel_rot.yaw:.2f}, R={rel_rot.roll:.2f}")

                        hand_sockets.append({
                            "name": socket_name,
                            "bone": str(bone_name),
                            "location": {"x": rel_loc.x, "y": rel_loc.y, "z": rel_loc.z},
                            "rotation": {"pitch": rel_rot.pitch, "yaw": rel_rot.yaw, "roll": rel_rot.roll}
                        })
            except Exception as e:
                log(f"    Error: {e}")

        if hand_sockets:
            results[mesh_path] = {"skeleton": skel_name, "sockets": hand_sockets}

    # Save results
    output_path = "C:/scripts/SLFConversion/migration_cache/character_sockets.json"
    with open(output_path, 'w') as f:
        json.dump(results, f, indent=2)
    log(f"\nSaved to: {output_path}")

if __name__ == "__main__":
    main()
