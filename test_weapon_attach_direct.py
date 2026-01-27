# test_weapon_attach_direct.py
# Spawn character, spawn weapon, attach directly to socket, log positions

import unreal
import traceback

CHARACTER_PATH = "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character"
WEAPON_BP_PATH = "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample01"
OUTPUT_FILE = "C:/scripts/slfconversion/attach_test_output.txt"

output_lines = []

def log(msg):
    print(f"[AttachTest] {msg}")
    unreal.log_warning(f"[AttachTest] {msg}")
    output_lines.append(msg)

def main():
    try:
        log("=" * 70)
        log("DIRECT WEAPON ATTACH TEST - SwordExample01")
        log("=" * 70)

        world = unreal.EditorLevelLibrary.get_editor_world()
        if not world:
            log("ERROR: No editor world")
            return

        # Load and spawn character
        char_bp = unreal.load_asset(CHARACTER_PATH)
        if not char_bp:
            log("ERROR: Could not load character BP")
            return

        char_class = char_bp.generated_class()
        if not char_class:
            log("ERROR: No generated class")
            return

        character = unreal.EditorLevelLibrary.spawn_actor_from_class(
            char_class,
            unreal.Vector(0, 0, 200),
            unreal.Rotator(0, 0, 0)
        )
        log(f"Spawned character: {character.get_name()}")

        # Get character mesh - try multiple approaches
        char_mesh = None

        # Try ACharacter.GetMesh()
        try:
            char_mesh = character.mesh
            log(f"Got mesh via character.mesh: {char_mesh.get_name() if char_mesh else 'None'}")
        except:
            pass

        if not char_mesh:
            # Try finding by class
            skel_meshes = character.get_components_by_class(unreal.SkeletalMeshComponent)
            log(f"Found {len(skel_meshes)} SkeletalMeshComponents")
            for m in skel_meshes:
                log(f"  - {m.get_name()}")
                if m.get_name() == "CharacterMesh0":
                    char_mesh = m
                    break
            if not char_mesh and len(skel_meshes) > 0:
                char_mesh = skel_meshes[0]

        if not char_mesh:
            log("ERROR: No skeletal mesh on character")
            return

        log(f"Using mesh: {char_mesh.get_name()}")

        # Log socket positions
        log("")
        log("=== SOCKET POSITIONS ON CHARACTER ===")

        hand_r_loc = None
        if char_mesh.does_socket_exist("hand_r"):
            hand_r_loc = char_mesh.get_socket_location("hand_r")
            hand_r_transform = char_mesh.get_socket_transform("hand_r", unreal.RelativeTransformSpace.RTS_WORLD)
            hand_r_rot = hand_r_transform.rotation.rotator()
            log(f"hand_r World: Loc=({hand_r_loc.x:.2f}, {hand_r_loc.y:.2f}, {hand_r_loc.z:.2f})")
            log(f"hand_r World: Rot=(P={hand_r_rot.pitch:.2f}, Y={hand_r_rot.yaw:.2f}, R={hand_r_rot.roll:.2f})")
        else:
            log("hand_r socket NOT found!")

        if char_mesh.does_socket_exist("weapon_r"):
            weapon_r_loc = char_mesh.get_socket_location("weapon_r")
            weapon_r_transform = char_mesh.get_socket_transform("weapon_r", unreal.RelativeTransformSpace.RTS_WORLD)
            weapon_r_rot = weapon_r_transform.rotation.rotator()
            log(f"weapon_r World: Loc=({weapon_r_loc.x:.2f}, {weapon_r_loc.y:.2f}, {weapon_r_loc.z:.2f})")
            log(f"weapon_r World: Rot=(P={weapon_r_rot.pitch:.2f}, Y={weapon_r_rot.yaw:.2f}, R={weapon_r_rot.roll:.2f})")

            if hand_r_loc:
                diff = weapon_r_loc - hand_r_loc
                log(f"Difference weapon_r - hand_r: ({diff.x:.2f}, {diff.y:.2f}, {diff.z:.2f})")
        else:
            log("weapon_r socket NOT found")

        # Load and spawn weapon
        log("")
        log("=== SPAWNING WEAPON ===")
        weapon_bp = unreal.load_asset(WEAPON_BP_PATH)
        if not weapon_bp:
            log("ERROR: Could not load weapon BP")
            return

        weapon_class = weapon_bp.generated_class()
        if not weapon_class:
            log("ERROR: No weapon generated class")
            return

        weapon = unreal.EditorLevelLibrary.spawn_actor_from_class(
            weapon_class,
            unreal.Vector(100, 0, 200),  # Spawn away from character initially
            unreal.Rotator(0, 0, 0)
        )
        log(f"Spawned weapon: {weapon.get_name()}")

        # Get weapon mesh component
        weapon_mesh = None
        static_meshes = weapon.get_components_by_class(unreal.StaticMeshComponent)
        log(f"Weapon has {len(static_meshes)} StaticMeshComponents")
        for comp in static_meshes:
            log(f"  - {comp.get_name()}")
            if "WeaponMesh" in comp.get_name() or "StaticMesh" in comp.get_name():
                weapon_mesh = comp
                break

        if weapon_mesh:
            rel_loc = weapon_mesh.get_editor_property('relative_location')
            rel_rot = weapon_mesh.get_editor_property('relative_rotation')
            log(f"WeaponMesh BEFORE attach:")
            log(f"  Relative: Loc=({rel_loc.x:.2f}, {rel_loc.y:.2f}, {rel_loc.z:.2f})")
            log(f"  Relative: Rot=(P={rel_rot.pitch:.2f}, Y={rel_rot.yaw:.2f}, R={rel_rot.roll:.2f})")

        # Manually attach weapon to hand_r socket
        log("")
        log("=== ATTACHING TO hand_r SOCKET ===")

        attach_rules = unreal.AttachmentTransformRules(
            unreal.AttachmentRule.SNAP_TO_TARGET,
            unreal.AttachmentRule.SNAP_TO_TARGET,
            unreal.AttachmentRule.SNAP_TO_TARGET,
            False
        )

        success = weapon.attach_to_component(
            char_mesh,
            attach_rules,
            "hand_r"
        )
        log(f"Attach result: {success}")

        # Log positions AFTER attachment
        log("")
        log("=== AFTER ATTACHMENT ===")

        actor_loc = weapon.get_actor_location()
        actor_rot = weapon.get_actor_rotation()
        log(f"Weapon Actor World: Loc=({actor_loc.x:.2f}, {actor_loc.y:.2f}, {actor_loc.z:.2f})")
        log(f"Weapon Actor World: Rot=(P={actor_rot.pitch:.2f}, Y={actor_rot.yaw:.2f}, R={actor_rot.roll:.2f})")

        if weapon_mesh:
            rel_loc = weapon_mesh.get_editor_property('relative_location')
            rel_rot = weapon_mesh.get_editor_property('relative_rotation')
            log(f"WeaponMesh Relative: Loc=({rel_loc.x:.2f}, {rel_loc.y:.2f}, {rel_loc.z:.2f})")
            log(f"WeaponMesh Relative: Rot=(P={rel_rot.pitch:.2f}, Y={rel_rot.yaw:.2f}, R={rel_rot.roll:.2f})")

            mesh_world_loc = weapon_mesh.get_component_location()
            mesh_world_rot = weapon_mesh.get_component_rotation()
            log(f"WeaponMesh World: Loc=({mesh_world_loc.x:.2f}, {mesh_world_loc.y:.2f}, {mesh_world_loc.z:.2f})")
            log(f"WeaponMesh World: Rot=(P={mesh_world_rot.pitch:.2f}, Y={mesh_world_rot.yaw:.2f}, R={mesh_world_rot.roll:.2f})")

            if hand_r_loc:
                offset_from_socket = mesh_world_loc - hand_r_loc
                log(f"WeaponMesh offset from hand_r socket: ({offset_from_socket.x:.2f}, {offset_from_socket.y:.2f}, {offset_from_socket.z:.2f})")

        # Check attach info
        root = weapon.get_root_component()
        if root:
            parent = root.get_attach_parent()
            socket = root.get_attach_socket_name()
            log(f"Attachment info: parent={parent.get_name() if parent else 'None'}, socket='{socket}'")

        log("")
        log("=== TEST COMPLETE ===")
        log("SwordExample01 expected offset: Z=36.49, R=180")

    except Exception as e:
        log(f"EXCEPTION: {e}")
        log(traceback.format_exc())

    finally:
        # Write output to file
        with open(OUTPUT_FILE, 'w') as f:
            f.write('\n'.join(output_lines))
        log(f"Output saved to {OUTPUT_FILE}")

if __name__ == "__main__":
    main()
