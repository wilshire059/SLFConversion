#!/usr/bin/env python3
"""Export skeleton assets as text to find socket data"""

import unreal
import re

def log(msg):
    unreal.log_warning(str(msg))

SKELETON_PATHS = [
    "/Game/SoulslikeFramework/Demo/SKM/Mannequins/Meshes/SK_Mannequin",
    "/Game/SoulslikeFramework/Demo/SKM/Mannequin_UE4/Meshes/SK_Mannequin_Skeleton",
]

def main():
    log("=" * 70)
    log("SKELETON TEXT EXPORT - LOOKING FOR SOCKETS")
    log("=" * 70)

    for path in SKELETON_PATHS:
        log(f"\n=== {path.split('/')[-1]} ===")

        try:
            export = unreal.EditorAssetLibrary.export_text(path)
            if export:
                # Search for socket-related content
                socket_matches = re.findall(r'(SocketName|hand_r|hand_l|weapon_r|weapon_l)[^)]*', export[:50000], re.IGNORECASE)
                if socket_matches:
                    log(f"Found socket references:")
                    for match in socket_matches[:20]:
                        log(f"  {match[:100]}")
                else:
                    log("No socket references found in export")

                # Also look for bone names
                bone_matches = re.findall(r'(hand|palm|wrist)[^"]*', export[:50000], re.IGNORECASE)
                if bone_matches:
                    log(f"\nFound hand-related bones:")
                    for match in set(bone_matches[:10]):
                        log(f"  {match[:50]}")
            else:
                log("Could not export")
        except Exception as e:
            log(f"Export error: {e}")

    # Also check the mesh component on a spawned character
    log("\n=== CHECKING SPAWNED CHARACTER MESH COMPONENT ===")

    char_path = "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character"
    bp = unreal.EditorAssetLibrary.load_asset(char_path)
    if bp:
        gen_class = bp.generated_class()
        if gen_class:
            world = unreal.EditorLevelLibrary.get_editor_world()
            if world:
                actor = unreal.EditorLevelLibrary.spawn_actor_from_class(gen_class, unreal.Vector(0, 0, 20000))
                if actor:
                    # Get mesh component
                    mesh_comps = actor.get_components_by_class(unreal.SkeletalMeshComponent)
                    for mesh_comp in mesh_comps:
                        log(f"\nMesh component: {mesh_comp.get_name()}")

                        # Try to get mesh
                        mesh = mesh_comp.get_editor_property("skeletal_mesh_asset")
                        log(f"  SkeletalMesh: {mesh.get_name() if mesh else 'None'}")

                        # Check specific sockets
                        for socket_name in ["hand_r", "hand_l", "weapon_r", "weapon_l", "RightHand", "LeftHand", "weapon_r_socket", "weapon_l_socket"]:
                            exists = mesh_comp.does_socket_exist(socket_name)
                            log(f"  Socket '{socket_name}': {'EXISTS' if exists else 'not found'}")

                            if exists:
                                # Get socket transform
                                transform = mesh_comp.get_socket_transform(socket_name, unreal.RelativeTransformSpace.COMPONENT)
                                loc = transform.translation
                                rot = transform.rotation.rotator()
                                log(f"    Location: X={loc.x:.2f}, Y={loc.y:.2f}, Z={loc.z:.2f}")
                                log(f"    Rotation: P={rot.pitch:.2f}, Y={rot.yaw:.2f}, R={rot.roll:.2f}")

                    actor.destroy_actor()

    log("\n" + "=" * 70)

if __name__ == "__main__":
    main()
