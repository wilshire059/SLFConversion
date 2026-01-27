# compare_hand_r_socket.py
# Compare hand_r socket position by spawning character

import unreal
import json

def log(msg):
    print(f"[Socket] {msg}")
    unreal.log_warning(f"[Socket] {msg}")

# Character path
CHARACTER_PATH = "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character"

def main():
    log("=" * 70)
    log("COMPARING HAND_R SOCKET POSITIONS VIA CHARACTER SPAWN")
    log("=" * 70)

    results = {}

    # Load character Blueprint
    log(f"\n--- Loading: {CHARACTER_PATH} ---")
    bp = unreal.load_asset(CHARACTER_PATH)
    if not bp:
        log(f"  Could not load character")
        return

    gen_class = bp.generated_class()
    if not gen_class:
        log(f"  No generated class")
        return

    # Spawn character
    try:
        world = unreal.EditorLevelLibrary.get_editor_world()
        if not world:
            log(f"  No world")
            return

        spawned = unreal.EditorLevelLibrary.spawn_actor_from_class(
            gen_class,
            unreal.Vector(0, 0, 1000),
            unreal.Rotator(0, 0, 0)
        )
        if not spawned:
            log(f"  Failed to spawn")
            return

        log(f"  Spawned: {spawned.get_name()}")

        # Find all skeletal mesh components
        mesh_comps = spawned.get_components_by_class(unreal.SkeletalMeshComponent)
        log(f"  Found {len(mesh_comps)} SkeletalMeshComponent(s)")

        for comp in mesh_comps:
            comp_name = comp.get_name()
            skel_mesh = comp.get_editor_property('skeletal_mesh_asset')
            mesh_name = skel_mesh.get_name() if skel_mesh else "NULL"

            log(f"\n  Component: {comp_name}")
            log(f"    Mesh: {mesh_name}")

            # Check for hand_r socket
            socket_names_to_check = ['hand_r', 'hand_l', 'weapon_r', 'weapon_l', 'YOURHAND']

            for socket_name in socket_names_to_check:
                if comp.does_socket_exist(socket_name):
                    # Get socket transform relative to the component
                    socket_transform_rel = comp.get_socket_transform(socket_name, unreal.RelativeTransformSpace.RTS_COMPONENT)
                    socket_transform_world = comp.get_socket_transform(socket_name, unreal.RelativeTransformSpace.RTS_WORLD)
                    socket_transform_parent = comp.get_socket_transform(socket_name, unreal.RelativeTransformSpace.RTS_PARENT_BONE_SPACE)

                    log(f"    Socket: {socket_name} EXISTS")
                    log(f"      COMPONENT-RELATIVE:")
                    log(f"        Location: {socket_transform_rel.translation}")
                    log(f"        Rotation: {socket_transform_rel.rotation.rotator()}")
                    log(f"      PARENT-BONE-RELATIVE:")
                    log(f"        Location: {socket_transform_parent.translation}")
                    log(f"        Rotation: {socket_transform_parent.rotation.rotator()}")

                    results[f"{comp_name}_{socket_name}"] = {
                        "component": comp_name,
                        "mesh": mesh_name,
                        "socket": socket_name,
                        "component_relative": {
                            "location": str(socket_transform_rel.translation),
                            "rotation": str(socket_transform_rel.rotation.rotator())
                        },
                        "parent_bone_relative": {
                            "location": str(socket_transform_parent.translation),
                            "rotation": str(socket_transform_parent.rotation.rotator())
                        }
                    }
                else:
                    pass  # Don't log non-existent sockets

            # Also check which sockets exist by trying common names
            all_possible = ['hand_r', 'hand_l', 'weapon_r', 'weapon_l', 'root', 'pelvis', 'spine_01', 'spine_02', 'spine_03', 'spine_04', 'spine_05', 'neck_01', 'neck_02', 'head', 'clavicle_l', 'clavicle_r', 'upperarm_l', 'upperarm_r', 'lowerarm_l', 'lowerarm_r', 'hand_l', 'hand_r', 'index_01_l', 'index_01_r', 'thigh_l', 'thigh_r']
            found_sockets = []
            for name in all_possible:
                if comp.does_socket_exist(name):
                    found_sockets.append(name)

            if 'hand_r' not in found_sockets:
                log(f"    WARNING: hand_r socket NOT found!")
                log(f"    Existing sockets (from test list): {found_sockets}")

        spawned.destroy_actor()
        log(f"\n  Destroyed spawned actor")

    except Exception as e:
        log(f"  Error: {e}")
        import traceback
        log(traceback.format_exc())

    # Save results
    output_path = "C:/scripts/SLFConversion/migration_cache/hand_r_socket_data.json"
    with open(output_path, 'w') as f:
        json.dump(results, f, indent=2)
    log(f"\nSaved to: {output_path}")

if __name__ == "__main__":
    main()
