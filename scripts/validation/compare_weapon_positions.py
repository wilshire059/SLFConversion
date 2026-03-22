# compare_weapon_positions.py
# Run in both bp_only and SLFConversion to compare weapon attachment positions
# Copy this file to both projects and run via UnrealEditor-Cmd

import unreal
import json
import os

# Configuration
CHARACTER_PATH = "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character"
WEAPON_DA_PATH = "/Game/SoulslikeFramework/Data/Items/DA_Katana"  # Test with Katana first
WEAPON_BP_PATH = "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Katana"

# Determine which project we're in
project_path = unreal.Paths.project_dir()
if "bp_only" in project_path.lower():
    PROJECT_NAME = "bp_only"
    OUTPUT_FILE = "C:/scripts/slfconversion/migration_cache/weapon_pos_bp_only.json"
else:
    PROJECT_NAME = "SLFConversion"
    OUTPUT_FILE = "C:/scripts/slfconversion/migration_cache/weapon_pos_slfconversion.json"

results = {
    "project": PROJECT_NAME,
    "tests": []
}

def log(msg):
    print(f"[WeaponCompare:{PROJECT_NAME}] {msg}")
    unreal.log_warning(f"[WeaponCompare:{PROJECT_NAME}] {msg}")

def get_vector_dict(v):
    return {"x": v.x, "y": v.y, "z": v.z}

def get_rotator_dict(r):
    return {"pitch": r.pitch, "yaw": r.yaw, "roll": r.roll}

def test_weapon(weapon_name, weapon_da_path, weapon_bp_path):
    """Test a single weapon and return position data"""
    log(f"")
    log(f"=" * 60)
    log(f"TESTING: {weapon_name}")
    log(f"=" * 60)

    test_result = {
        "weapon_name": weapon_name,
        "weapon_da": weapon_da_path,
        "weapon_bp": weapon_bp_path,
        "success": False,
        "error": None
    }

    try:
        # Get editor world
        world = unreal.EditorLevelLibrary.get_editor_world()
        if not world:
            test_result["error"] = "No editor world"
            log("ERROR: No editor world")
            return test_result

        # Load and spawn character
        char_bp = unreal.load_asset(CHARACTER_PATH)
        if not char_bp:
            test_result["error"] = "Could not load character Blueprint"
            log("ERROR: Could not load character Blueprint")
            return test_result

        char_class = char_bp.generated_class()
        character = unreal.EditorLevelLibrary.spawn_actor_from_class(
            char_class,
            unreal.Vector(0, 0, 500),
            unreal.Rotator(0, 0, 0)
        )
        log(f"Spawned character: {character.get_name()}")
        test_result["character_name"] = character.get_name()

        # Get character mesh
        char_mesh = character.mesh if hasattr(character, 'mesh') else None
        if not char_mesh:
            skel_meshes = character.get_components_by_class(unreal.SkeletalMeshComponent)
            for m in skel_meshes:
                if "CharacterMesh" in m.get_name():
                    char_mesh = m
                    break

        if not char_mesh:
            test_result["error"] = "No character mesh found"
            log("ERROR: No character mesh")
            character.destroy_actor()
            return test_result

        log(f"Using character mesh: {char_mesh.get_name()}")

        # Log socket positions BEFORE equipping
        test_result["sockets"] = {}
        for socket_name in ["hand_r", "hand_l", "weapon_r", "weapon_l"]:
            if char_mesh.does_socket_exist(socket_name):
                socket_loc = char_mesh.get_socket_location(socket_name)
                socket_transform = char_mesh.get_socket_transform(socket_name, unreal.RelativeTransformSpace.RTS_WORLD)
                socket_rot = socket_transform.rotation.rotator()
                test_result["sockets"][socket_name] = {
                    "exists": True,
                    "world_location": get_vector_dict(socket_loc),
                    "world_rotation": get_rotator_dict(socket_rot)
                }
                log(f"Socket {socket_name}: Loc={socket_loc}, Rot=P{socket_rot.pitch:.1f} Y{socket_rot.yaw:.1f} R{socket_rot.roll:.1f}")
            else:
                test_result["sockets"][socket_name] = {"exists": False}
                log(f"Socket {socket_name}: NOT FOUND")

        # Get EquipmentManager component
        equip_manager = None
        components = character.get_components_by_class(unreal.ActorComponent)
        for comp in components:
            class_name = comp.get_class().get_name()
            if "EquipmentManager" in class_name:
                equip_manager = comp
                log(f"Found EquipmentManager: {comp.get_name()} ({class_name})")
                break

        if not equip_manager:
            test_result["error"] = "No EquipmentManager found"
            log("ERROR: No EquipmentManager")
            character.destroy_actor()
            return test_result

        # Load weapon data asset
        weapon_da = unreal.load_asset(weapon_da_path)
        if not weapon_da:
            test_result["error"] = f"Could not load weapon DA: {weapon_da_path}"
            log(f"ERROR: Could not load {weapon_da_path}")
            character.destroy_actor()
            return test_result

        log(f"Loaded weapon DA: {weapon_da.get_name()}")

        # Get right hand slot tag
        right_hand_slot = unreal.GameplayTag.request_gameplay_tag(
            "SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 1"
        )

        # Try to equip weapon
        log(f"Attempting to equip {weapon_da.get_name()} to right hand...")
        try:
            # Try multiple methods
            equipped = False

            # Method 1: EquipWeaponToSlot
            try:
                result = equip_manager.equip_weapon_to_slot(weapon_da, right_hand_slot, True)
                log(f"equip_weapon_to_slot result: {result}")
                equipped = True
            except Exception as e1:
                log(f"equip_weapon_to_slot failed: {e1}")

            # Method 2: EquipItemToSlot
            if not equipped:
                try:
                    equip_manager.equip_item_to_slot(weapon_da, right_hand_slot, True)
                    log("equip_item_to_slot succeeded")
                    equipped = True
                except Exception as e2:
                    log(f"equip_item_to_slot failed: {e2}")

            test_result["equip_attempted"] = True
            test_result["equip_succeeded"] = equipped

        except Exception as e:
            test_result["equip_error"] = str(e)
            log(f"Equip exception: {e}")

        # Find spawned weapon actors
        log("")
        log("Searching for spawned weapon actors...")
        all_actors = unreal.GameplayStatics.get_all_actors_of_class(world, unreal.Actor)

        weapon_actors = []
        for actor in all_actors:
            actor_name = actor.get_name()
            actor_class = actor.get_class().get_name()
            if "Weapon" in actor_name or "Weapon" in actor_class:
                if "Katana" in actor_name or "Sword" in actor_name or "Greatsword" in actor_name:
                    weapon_actors.append(actor)
                    log(f"  Found weapon: {actor_name} ({actor_class})")

        test_result["weapons_found"] = []

        for weapon in weapon_actors:
            weapon_data = {
                "name": weapon.get_name(),
                "class": weapon.get_class().get_name()
            }

            # Actor transform
            actor_loc = weapon.get_actor_location()
            actor_rot = weapon.get_actor_rotation()
            weapon_data["actor_world_location"] = get_vector_dict(actor_loc)
            weapon_data["actor_world_rotation"] = get_rotator_dict(actor_rot)
            log(f"")
            log(f"--- {weapon.get_name()} ---")
            log(f"  Actor World Loc: {actor_loc}")
            log(f"  Actor World Rot: P{actor_rot.pitch:.1f} Y{actor_rot.yaw:.1f} R{actor_rot.roll:.1f}")

            # Find mesh component
            static_meshes = weapon.get_components_by_class(unreal.StaticMeshComponent)
            for mesh in static_meshes:
                mesh_name = mesh.get_name()
                if "WeaponMesh" in mesh_name or "StaticMesh" in mesh_name:
                    # Relative transform
                    rel_loc = mesh.get_editor_property('relative_location')
                    rel_rot = mesh.get_editor_property('relative_rotation')
                    weapon_data["mesh_name"] = mesh_name
                    weapon_data["mesh_relative_location"] = get_vector_dict(rel_loc)
                    weapon_data["mesh_relative_rotation"] = get_rotator_dict(rel_rot)
                    log(f"  Mesh Relative Loc: {rel_loc}")
                    log(f"  Mesh Relative Rot: P{rel_rot.pitch:.1f} Y{rel_rot.yaw:.1f} R{rel_rot.roll:.1f}")

                    # World transform
                    world_loc = mesh.get_component_location()
                    world_rot = mesh.get_component_rotation()
                    weapon_data["mesh_world_location"] = get_vector_dict(world_loc)
                    weapon_data["mesh_world_rotation"] = get_rotator_dict(world_rot)
                    log(f"  Mesh World Loc: {world_loc}")
                    log(f"  Mesh World Rot: P{world_rot.pitch:.1f} Y{world_rot.yaw:.1f} R{world_rot.roll:.1f}")

                    break

            # Check attachment
            root = weapon.get_root_component()
            if root:
                parent = root.get_attach_parent()
                if parent:
                    socket_name = root.get_attach_socket_name()
                    weapon_data["attached_to"] = parent.get_name()
                    weapon_data["attached_socket"] = str(socket_name)
                    log(f"  Attached to: {parent.get_name()} socket '{socket_name}'")

                    # Get socket world position
                    if parent.does_socket_exist(socket_name):
                        socket_loc = parent.get_socket_location(socket_name)
                        weapon_data["socket_world_location"] = get_vector_dict(socket_loc)
                        log(f"  Socket World Loc: {socket_loc}")

                        # Calculate offset from socket to mesh
                        if "mesh_world_location" in weapon_data:
                            mesh_world = unreal.Vector(
                                weapon_data["mesh_world_location"]["x"],
                                weapon_data["mesh_world_location"]["y"],
                                weapon_data["mesh_world_location"]["z"]
                            )
                            offset = mesh_world - socket_loc
                            weapon_data["offset_from_socket"] = get_vector_dict(offset)
                            log(f"  OFFSET from socket: {offset}")
                else:
                    weapon_data["attached_to"] = None
                    log(f"  NOT attached")

            test_result["weapons_found"].append(weapon_data)

        # Also log hand_r socket position if it exists
        if char_mesh.does_socket_exist("hand_r") and len(test_result["weapons_found"]) > 0:
            hand_r_loc = char_mesh.get_socket_location("hand_r")
            log(f"")
            log(f"hand_r socket position: {hand_r_loc}")

            # Calculate offset for each weapon
            for weapon_data in test_result["weapons_found"]:
                if "mesh_world_location" in weapon_data:
                    mesh_loc = unreal.Vector(
                        weapon_data["mesh_world_location"]["x"],
                        weapon_data["mesh_world_location"]["y"],
                        weapon_data["mesh_world_location"]["z"]
                    )
                    offset_from_hand = mesh_loc - hand_r_loc
                    weapon_data["offset_from_hand_r"] = get_vector_dict(offset_from_hand)
                    log(f"  {weapon_data['name']} offset from hand_r: {offset_from_hand}")

        test_result["success"] = True

        # Clean up
        character.destroy_actor()
        for weapon in weapon_actors:
            if weapon.is_valid():
                weapon.destroy_actor()

    except Exception as e:
        test_result["error"] = str(e)
        log(f"EXCEPTION: {e}")
        import traceback
        log(traceback.format_exc())

    return test_result

def main():
    log("=" * 70)
    log(f"WEAPON POSITION COMPARISON TEST - {PROJECT_NAME}")
    log("=" * 70)

    # Test Katana
    katana_result = test_weapon(
        "Katana",
        "/Game/SoulslikeFramework/Data/Items/DA_Katana",
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Katana"
    )
    results["tests"].append(katana_result)

    # Test SwordExample01
    sword01_result = test_weapon(
        "SwordExample01",
        "/Game/SoulslikeFramework/Data/Items/DA_Sword01",
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample01"
    )
    results["tests"].append(sword01_result)

    # Save results
    os.makedirs(os.path.dirname(OUTPUT_FILE), exist_ok=True)
    with open(OUTPUT_FILE, 'w') as f:
        json.dump(results, f, indent=2)

    log("")
    log(f"Results saved to: {OUTPUT_FILE}")
    log("=" * 70)

if __name__ == "__main__":
    main()
