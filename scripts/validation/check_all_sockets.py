# check_all_sockets.py
# Check all skeletal mesh components on character for sockets

import unreal

CHARACTER_PATH = "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character"

def log(msg):
    print(f"[SocketScan] {msg}")
    unreal.log_warning(f"[SocketScan] {msg}")

def main():
    log("=" * 70)
    log("SCANNING ALL SKELETAL MESH COMPONENTS FOR SOCKETS")
    log("=" * 70)

    world = unreal.EditorLevelLibrary.get_editor_world()
    if not world:
        log("ERROR: No editor world")
        return

    char_bp = unreal.load_asset(CHARACTER_PATH)
    char_class = char_bp.generated_class()

    character = unreal.EditorLevelLibrary.spawn_actor_from_class(
        char_class,
        unreal.Vector(0, 0, 200),
        unreal.Rotator(0, 0, 0)
    )
    log(f"Spawned: {character.get_name()}")

    # Get ALL skeletal mesh components
    skel_meshes = character.get_components_by_class(unreal.SkeletalMeshComponent)
    log(f"\nFound {len(skel_meshes)} SkeletalMeshComponent(s)")

    for mesh in skel_meshes:
        mesh_name = mesh.get_name()
        skel_asset = mesh.get_editor_property('skeletal_mesh_asset')
        asset_name = skel_asset.get_name() if skel_asset else "NULL"

        log(f"\n--- {mesh_name} (Asset: {asset_name}) ---")

        # Check for specific sockets
        target_sockets = ["hand_r", "hand_l", "weapon_r", "weapon_l", "Hand_R", "Hand_L"]
        found_any = False

        for socket_name in target_sockets:
            if mesh.does_socket_exist(socket_name):
                found_any = True
                loc = mesh.get_socket_location(socket_name)
                transform = mesh.get_socket_transform(socket_name, unreal.RelativeTransformSpace.RTS_WORLD)
                rot = transform.rotation.rotator()
                log(f"  FOUND: {socket_name}")
                log(f"    World Loc: ({loc.x:.2f}, {loc.y:.2f}, {loc.z:.2f})")
                log(f"    World Rot: (P={rot.pitch:.2f}, Y={rot.yaw:.2f}, R={rot.roll:.2f})")

        if not found_any:
            log("  No hand/weapon sockets found")

            # List all sockets on this mesh
            try:
                if skel_asset:
                    skeleton = skel_asset.skeleton
                    if skeleton:
                        # Can't easily iterate sockets from Python, but we can check specific names
                        log("  Checking common socket names...")
                        common_sockets = [
                            "head", "neck", "spine", "pelvis", "root",
                            "clavicle_l", "clavicle_r", "upperarm_l", "upperarm_r",
                            "lowerarm_l", "lowerarm_r", "hand_r", "hand_l",
                            "thigh_l", "thigh_r", "calf_l", "calf_r",
                            "foot_l", "foot_r", "ball_l", "ball_r"
                        ]
                        for s in common_sockets:
                            if mesh.does_socket_exist(s):
                                log(f"    Found socket: {s}")
            except:
                pass

    # Also check if there's a specific "Arms" mesh component
    log("\n=== SEARCHING FOR ARMS/HAND MESH ===")
    all_comps = character.get_components_by_class(unreal.SceneComponent)
    for comp in all_comps:
        name = comp.get_name().lower()
        if "arm" in name or "hand" in name:
            log(f"  Component: {comp.get_name()} ({comp.get_class().get_name()})")

    character.destroy_actor()

if __name__ == "__main__":
    main()
