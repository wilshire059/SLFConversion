# compare_socket_positions.py
# Compare hand_r socket position between projects

import unreal
import json

def log(msg):
    print(f"[Socket] {msg}")
    unreal.log_warning(f"[Socket] {msg}")

# Common character paths to check
CHARACTER_PATHS = [
    "/Game/SoulslikeFramework/Blueprints/_Characters/Player/B_Soulslike_Character",
]

# Skeleton paths to check directly
SKELETON_PATHS = [
    "/Game/SoulslikeFramework/Demo/_Character/Meshes/SKM_Manny",
    "/Game/SoulslikeFramework/Demo/_Character/Meshes/SKM_Quinn",
]

def main():
    log("=" * 70)
    log("CHECKING HAND_R SOCKET POSITIONS")
    log("=" * 70)

    results = {}

    # Check character Blueprint's mesh
    for char_path in CHARACTER_PATHS:
        log(f"\n--- Character: {char_path} ---")

        bp = unreal.load_asset(char_path)
        if not bp:
            log(f"  Could not load")
            continue

        gen_class = bp.generated_class()
        if not gen_class:
            continue

        cdo = unreal.get_default_object(gen_class)
        if not cdo:
            continue

        # Get the skeletal mesh component
        mesh_comp = cdo.get_components_by_class(unreal.SkeletalMeshComponent)
        log(f"  Found {len(mesh_comp)} SkeletalMeshComponent(s)")

        for comp in mesh_comp:
            comp_name = comp.get_name()
            skel_mesh = comp.get_editor_property('skeletal_mesh_asset')

            if skel_mesh:
                mesh_name = skel_mesh.get_name()
                log(f"  Component: {comp_name}, Mesh: {mesh_name}")

                # Check socket
                if comp.does_socket_exist('hand_r'):
                    socket_transform = comp.get_socket_transform('hand_r', unreal.RelativeTransformSpace.RELATIVE_TO_PARENT)
                    log(f"    hand_r socket Location: {socket_transform.translation}")
                    log(f"    hand_r socket Rotation: {socket_transform.rotation.rotator()}")

                    results[f"{char_path}_{comp_name}"] = {
                        "mesh": mesh_name,
                        "hand_r_location": str(socket_transform.translation),
                        "hand_r_rotation": str(socket_transform.rotation.rotator())
                    }
                else:
                    log(f"    hand_r socket NOT FOUND")

                    # List available sockets
                    skeleton = skel_mesh.get_editor_property('skeleton')
                    if skeleton:
                        socket_count = skeleton.num_sockets()
                        log(f"    Skeleton has {socket_count} sockets")
                        # Try to get socket names
                        for i in range(min(socket_count, 20)):
                            try:
                                socket = skeleton.get_socket_by_index(i)
                                if socket:
                                    log(f"      Socket {i}: {socket.get_name()}")
                            except:
                                pass
            else:
                log(f"  Component: {comp_name}, Mesh: NULL")

    # Check skeletons directly
    for skel_path in SKELETON_PATHS:
        log(f"\n--- Skeleton: {skel_path} ---")

        skel_mesh = unreal.load_asset(skel_path)
        if not skel_mesh:
            log(f"  Could not load")
            continue

        # Get skeleton
        skeleton = skel_mesh.get_editor_property('skeleton')
        if not skeleton:
            log(f"  No skeleton")
            continue

        skel_name = skeleton.get_name()
        log(f"  Skeleton: {skel_name}")

        # Check for hand_r socket on skeleton
        socket_count = skeleton.num_sockets()
        log(f"  Socket count: {socket_count}")

        hand_sockets = []
        for i in range(socket_count):
            try:
                socket = skeleton.get_socket_by_index(i)
                if socket:
                    socket_name = socket.get_name()
                    if 'hand' in socket_name.lower() or 'weapon' in socket_name.lower():
                        # Get socket transform relative to bone
                        rel_loc = socket.get_editor_property('relative_location')
                        rel_rot = socket.get_editor_property('relative_rotation')
                        bone_name = socket.get_editor_property('bone_name')
                        log(f"    Socket: {socket_name}")
                        log(f"      Bone: {bone_name}")
                        log(f"      RelLoc: X={rel_loc.x:.2f}, Y={rel_loc.y:.2f}, Z={rel_loc.z:.2f}")
                        log(f"      RelRot: P={rel_rot.pitch:.2f}, Y={rel_rot.yaw:.2f}, R={rel_rot.roll:.2f}")

                        hand_sockets.append({
                            "name": socket_name,
                            "bone": str(bone_name),
                            "location": {"x": rel_loc.x, "y": rel_loc.y, "z": rel_loc.z},
                            "rotation": {"pitch": rel_rot.pitch, "yaw": rel_rot.yaw, "roll": rel_rot.roll}
                        })
            except Exception as e:
                pass

        results[skel_path] = {"skeleton": skel_name, "hand_sockets": hand_sockets}

    # Save results
    output_path = "C:/scripts/SLFConversion/migration_cache/socket_comparison.json"
    with open(output_path, 'w') as f:
        json.dump(results, f, indent=2)
    log(f"\nSaved to: {output_path}")

if __name__ == "__main__":
    main()
