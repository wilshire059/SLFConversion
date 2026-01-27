#!/usr/bin/env python3
"""Check sockets on character skeletal meshes at runtime"""

import unreal
import json

def log(msg):
    unreal.log_warning(str(msg))

MESH_PATHS = [
    "/Game/SoulslikeFramework/Demo/SKM/Mannequins/Meshes/SKM_Quinn",
    "/Game/SoulslikeFramework/Demo/SKM/Mannequins/Meshes/SKM_Manny",
]

def check_socket_on_mesh(mesh_path):
    """Check sockets directly on a skeletal mesh"""
    log(f"\n=== {mesh_path.split('/')[-1]} ===")

    mesh = unreal.EditorAssetLibrary.load_asset(mesh_path)
    if not mesh:
        log(f"ERROR: Could not load mesh")
        return None

    result = {
        "path": mesh_path,
        "class": mesh.get_class().get_name(),
        "sockets": [],
        "skeleton": None,
        "bones": []
    }

    # Get skeleton
    try:
        skeleton = mesh.get_editor_property("skeleton")
        if skeleton:
            result["skeleton"] = skeleton.get_path_name()
            log(f"Skeleton: {skeleton.get_name()}")

            # Try to get sockets from skeleton
            try:
                sockets = skeleton.get_editor_property("sockets")
                if sockets:
                    log(f"Found {len(sockets)} sockets on skeleton:")
                    for socket in sockets:
                        socket_name = str(socket.get_editor_property("socket_name"))
                        bone_name = str(socket.get_editor_property("bone_name"))

                        socket_data = {
                            "name": socket_name,
                            "bone": bone_name,
                        }

                        try:
                            loc = socket.get_editor_property("relative_location")
                            socket_data["location"] = {"x": loc.x, "y": loc.y, "z": loc.z}
                        except: pass

                        try:
                            rot = socket.get_editor_property("relative_rotation")
                            socket_data["rotation"] = {"pitch": rot.pitch, "yaw": rot.yaw, "roll": rot.roll}
                        except: pass

                        try:
                            scale = socket.get_editor_property("relative_scale")
                            socket_data["scale"] = {"x": scale.x, "y": scale.y, "z": scale.z}
                        except: pass

                        result["sockets"].append(socket_data)
                        log(f"  - {socket_name} (bone: {bone_name})")
                        if "location" in socket_data:
                            loc = socket_data["location"]
                            log(f"    Location: X={loc['x']:.2f}, Y={loc['y']:.2f}, Z={loc['z']:.2f}")
                        if "rotation" in socket_data:
                            rot = socket_data["rotation"]
                            log(f"    Rotation: P={rot['pitch']:.2f}, Y={rot['yaw']:.2f}, R={rot['roll']:.2f}")
                else:
                    log("No sockets found on skeleton")
            except Exception as e:
                log(f"Error reading sockets: {e}")

            # Get bone names (look for hand-related)
            try:
                ref_skeleton = skeleton.get_editor_property("reference_skeleton")
                if ref_skeleton:
                    # UE5.7+ may not expose bone data this way
                    pass
            except: pass
    except Exception as e:
        log(f"Error getting skeleton: {e}")

    # Try getting sockets directly from mesh
    log("\nChecking mesh asset sockets:")
    try:
        mesh_sockets = mesh.get_editor_property("mesh_socket_list")
        if mesh_sockets:
            log(f"Found {len(mesh_sockets)} mesh sockets")
            for s in mesh_sockets:
                log(f"  - {s}")
    except Exception as e:
        log(f"mesh_socket_list not available: {e}")

    return result

def spawn_and_check_runtime():
    """Spawn a temp mesh component and check sockets at runtime"""
    log("\n=== RUNTIME SOCKET CHECK ===")

    world = unreal.EditorLevelLibrary.get_editor_world()
    if not world:
        log("No editor world")
        return

    for mesh_path in MESH_PATHS:
        mesh = unreal.EditorAssetLibrary.load_asset(mesh_path)
        if not mesh:
            continue

        log(f"\n--- Runtime check: {mesh_path.split('/')[-1]} ---")

        # Spawn temp actor
        temp_actor = unreal.EditorLevelLibrary.spawn_actor_from_class(
            unreal.Actor, unreal.Vector(0, 0, 30000)
        )

        if temp_actor:
            # Add skeletal mesh component
            skm_comp = unreal.SkeletalMeshComponent(temp_actor)
            skm_comp.set_editor_property("skeletal_mesh_asset", mesh)
            temp_actor.add_component_by_class(unreal.SkeletalMeshComponent)

            comps = temp_actor.get_components_by_class(unreal.SkeletalMeshComponent)
            for comp in comps:
                comp.set_editor_property("skeletal_mesh_asset", mesh)

                # Check known socket names
                SOCKET_NAMES = [
                    "hand_r", "hand_l",
                    "weapon_r", "weapon_l",
                    "RightHand", "LeftHand",
                    "hand_rSocket", "hand_lSocket",
                    "VirtualBone_Weapon_R", "VirtualBone_Weapon_L"
                ]

                log(f"Socket existence check on {mesh_path.split('/')[-1]}:")
                for socket_name in SOCKET_NAMES:
                    exists = comp.does_socket_exist(socket_name)
                    if exists:
                        transform = comp.get_socket_transform(socket_name, unreal.RelativeTransformSpace.COMPONENT)
                        loc = transform.translation
                        rot = transform.rotation.rotator()
                        log(f"  '{socket_name}': EXISTS")
                        log(f"    Location: X={loc.x:.2f}, Y={loc.y:.2f}, Z={loc.z:.2f}")
                        log(f"    Rotation: P={rot.pitch:.2f}, Y={rot.yaw:.2f}, R={rot.roll:.2f}")
                    else:
                        log(f"  '{socket_name}': NOT FOUND")

                # Get all socket names
                try:
                    # This might work to list all sockets
                    mesh_asset = comp.get_editor_property("skeletal_mesh_asset")
                    if mesh_asset:
                        skeleton = mesh_asset.get_editor_property("skeleton")
                        if skeleton:
                            sockets = skeleton.get_editor_property("sockets")
                            if sockets:
                                log(f"\nAll sockets on skeleton:")
                                for socket in sockets:
                                    name = str(socket.get_editor_property("socket_name"))
                                    bone = str(socket.get_editor_property("bone_name"))
                                    log(f"  {name} -> {bone}")
                except Exception as e:
                    log(f"Error listing sockets: {e}")

            temp_actor.destroy_actor()

def main():
    log("=" * 70)
    log("MESH SOCKET CHECK")
    log("=" * 70)

    results = {}

    for path in MESH_PATHS:
        result = check_socket_on_mesh(path)
        if result:
            results[path.split("/")[-1]] = result

    spawn_and_check_runtime()

    # Save results
    output_path = "C:/scripts/slfconversion/migration_cache/mesh_sockets.json"
    with open(output_path, 'w') as f:
        json.dump(results, f, indent=2)
    log(f"\nSaved to: {output_path}")

    log("\n" + "=" * 70)

if __name__ == "__main__":
    main()
