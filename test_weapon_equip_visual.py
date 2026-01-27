# test_weapon_equip_visual.py
# Spawn character, equip weapon, and log world positions for comparison

import unreal
import time

CHARACTER_PATH = "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character"
WEAPON_DATA_ASSET = "/Game/SoulslikeFramework/Data/Items/DA_Sword01"  # SwordExample01

def log(msg):
    print(f"[EquipTest] {msg}")
    unreal.log_warning(f"[EquipTest] {msg}")

def main():
    log("=" * 70)
    log("WEAPON EQUIP VISUAL TEST")
    log("=" * 70)

    # Get editor world
    world = unreal.EditorLevelLibrary.get_editor_world()
    if not world:
        log("ERROR: No editor world")
        return

    # Load character Blueprint
    char_bp = unreal.load_asset(CHARACTER_PATH)
    if not char_bp:
        log("ERROR: Could not load character Blueprint")
        return

    char_class = char_bp.generated_class()
    if not char_class:
        log("ERROR: No generated class for character")
        return

    # Spawn character
    spawn_location = unreal.Vector(0, 0, 200)
    spawn_rotation = unreal.Rotator(0, 0, 0)

    character = unreal.EditorLevelLibrary.spawn_actor_from_class(
        char_class,
        spawn_location,
        spawn_rotation
    )

    if not character:
        log("ERROR: Failed to spawn character")
        return

    log(f"Spawned character: {character.get_name()}")

    # Get EquipmentManager component
    equip_manager = None
    components = character.get_components_by_class(unreal.ActorComponent)
    for comp in components:
        if "EquipmentManager" in comp.get_class().get_name():
            equip_manager = comp
            log(f"Found EquipmentManager: {comp.get_name()} ({comp.get_class().get_name()})")
            break

    if not equip_manager:
        log("ERROR: No EquipmentManager found on character")
        # Try to get from PlayerController instead
        log("Attempting alternate equip method...")

    # Load weapon data asset
    weapon_da = unreal.load_asset(WEAPON_DATA_ASSET)
    if not weapon_da:
        log("ERROR: Could not load weapon data asset")
        character.destroy_actor()
        return

    log(f"Loaded weapon DA: {weapon_da.get_name()}")

    # Get the right hand slot tag
    right_hand_slot = unreal.GameplayTag.request_gameplay_tag("SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 1")
    log(f"Right hand slot tag: {right_hand_slot}")

    # Try to equip via interface or direct call
    if equip_manager:
        try:
            # Try calling EquipWeaponToSlot
            log("Calling EquipWeaponToSlot...")
            result = equip_manager.equip_weapon_to_slot(weapon_da, right_hand_slot, True)
            log(f"EquipWeaponToSlot result: {result}")
        except Exception as e:
            log(f"EquipWeaponToSlot failed: {e}")

            # Try alternate method
            try:
                log("Trying alternate equip method...")
                equip_manager.equip_item_to_slot(weapon_da, right_hand_slot, True)
            except Exception as e2:
                log(f"Alternate method failed: {e2}")

    # Wait a frame for spawning to complete
    # Note: In editor scripting we can't really wait, but let's try to find the weapon

    # Find spawned weapon actor
    log("\nSearching for spawned weapon actors...")
    all_actors = unreal.GameplayStatics.get_all_actors_of_class(world, unreal.Actor)

    weapon_actors = []
    for actor in all_actors:
        actor_name = actor.get_name()
        if "Weapon" in actor_name and "Sword" in actor_name:
            weapon_actors.append(actor)
            log(f"  Found: {actor_name} ({actor.get_class().get_name()})")

    # Log weapon mesh world positions
    for weapon in weapon_actors:
        log(f"\n--- {weapon.get_name()} ---")

        # Actor world transform
        actor_loc = weapon.get_actor_location()
        actor_rot = weapon.get_actor_rotation()
        log(f"  Actor World Location: X={actor_loc.x:.2f}, Y={actor_loc.y:.2f}, Z={actor_loc.z:.2f}")
        log(f"  Actor World Rotation: P={actor_rot.pitch:.2f}, Y={actor_rot.yaw:.2f}, R={actor_rot.roll:.2f}")

        # Find StaticMeshComponent
        mesh_comps = weapon.get_components_by_class(unreal.StaticMeshComponent)
        for mesh in mesh_comps:
            if mesh.get_name() == "WeaponMesh" or "StaticMesh" in mesh.get_name():
                # Relative transform
                rel_loc = mesh.get_editor_property('relative_location')
                rel_rot = mesh.get_editor_property('relative_rotation')
                log(f"  Mesh Relative Location: X={rel_loc.x:.2f}, Y={rel_loc.y:.2f}, Z={rel_loc.z:.2f}")
                log(f"  Mesh Relative Rotation: P={rel_rot.pitch:.2f}, Y={rel_rot.yaw:.2f}, R={rel_rot.roll:.2f}")

                # World transform of mesh component
                world_transform = mesh.get_component_location()
                world_rot = mesh.get_component_rotation()
                log(f"  Mesh World Location: X={world_transform.x:.2f}, Y={world_transform.y:.2f}, Z={world_transform.z:.2f}")
                log(f"  Mesh World Rotation: P={world_rot.pitch:.2f}, Y={world_rot.yaw:.2f}, R={world_rot.roll:.2f}")

        # Check attach parent
        root = weapon.get_root_component()
        if root:
            parent = root.get_attach_parent()
            if parent:
                socket = root.get_attach_socket_name()
                log(f"  Attached to: {parent.get_name()} socket '{socket}'")

                # Get socket world location
                socket_loc = parent.get_socket_location(socket)
                socket_rot = parent.get_socket_transform(socket, unreal.RelativeTransformSpace.RTS_WORLD).rotation.rotator()
                log(f"  Socket World Location: X={socket_loc.x:.2f}, Y={socket_loc.y:.2f}, Z={socket_loc.z:.2f}")
                log(f"  Socket World Rotation: P={socket_rot.pitch:.2f}, Y={socket_rot.yaw:.2f}, R={socket_rot.roll:.2f}")
            else:
                log(f"  NOT attached to any parent")

    # Log character hand position for reference
    char_mesh = character.get_component_by_class(unreal.SkeletalMeshComponent)
    if char_mesh:
        hand_r_loc = char_mesh.get_socket_location("hand_r")
        weapon_r_loc = char_mesh.get_socket_location("weapon_r") if char_mesh.does_socket_exist("weapon_r") else None

        log(f"\nCharacter Socket Positions:")
        log(f"  hand_r: X={hand_r_loc.x:.2f}, Y={hand_r_loc.y:.2f}, Z={hand_r_loc.z:.2f}")
        if weapon_r_loc:
            log(f"  weapon_r: X={weapon_r_loc.x:.2f}, Y={weapon_r_loc.y:.2f}, Z={weapon_r_loc.z:.2f}")

            # Calculate difference
            diff = weapon_r_loc - hand_r_loc
            log(f"  Difference (weapon_r - hand_r): X={diff.x:.2f}, Y={diff.y:.2f}, Z={diff.z:.2f}")

    # Don't destroy - leave spawned for visual inspection
    log("\nCharacter and weapons left spawned for visual inspection.")
    log("Check the editor viewport to see weapon positioning.")

if __name__ == "__main__":
    main()
