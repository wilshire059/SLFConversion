# spawn_and_check_weapon.py
# Spawn a weapon actor and check its actual runtime transform

import unreal

def log(msg):
    print(f"[SpawnCheck] {msg}")
    unreal.log_warning(f"[SpawnCheck] {msg}")

BP_PATH = "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample01"

def main():
    log("=" * 70)
    log("SPAWNING WEAPON AND CHECKING RUNTIME TRANSFORM")
    log("=" * 70)

    # Get the world
    world = unreal.EditorLevelLibrary.get_editor_world()
    if not world:
        log("ERROR: No editor world")
        return

    # Load the Blueprint class
    bp = unreal.load_asset(BP_PATH)
    if not bp:
        log(f"ERROR: Could not load {BP_PATH}")
        return

    gen_class = bp.generated_class()
    log(f"Blueprint class: {gen_class.get_name()}")

    # Check CDO first
    log("\n--- CDO CHECK ---")
    cdo = unreal.get_default_object(gen_class)
    try:
        cdo_mesh = cdo.get_editor_property('weapon_mesh')
        if cdo_mesh:
            cdo_loc = cdo_mesh.get_editor_property('relative_location')
            cdo_rot = cdo_mesh.get_editor_property('relative_rotation')
            log(f"CDO WeaponMesh Location: X={cdo_loc.x:.4f}, Y={cdo_loc.y:.4f}, Z={cdo_loc.z:.4f}")
            log(f"CDO WeaponMesh Rotation: P={cdo_rot.pitch:.2f}, Y={cdo_rot.yaw:.2f}, R={cdo_rot.roll:.2f}")
    except Exception as e:
        log(f"CDO check error: {e}")

    # Spawn the actor
    log("\n--- SPAWNING ACTOR ---")
    spawn_location = unreal.Vector(0, 0, 100)
    spawn_rotation = unreal.Rotator(0, 0, 0)

    # Use spawn actor from class
    spawned_actor = unreal.EditorLevelLibrary.spawn_actor_from_class(
        gen_class,
        spawn_location,
        spawn_rotation
    )

    if not spawned_actor:
        log("ERROR: Failed to spawn actor")
        return

    log(f"Spawned: {spawned_actor.get_name()}")

    # Check all components
    log("\n--- SPAWNED ACTOR COMPONENTS ---")
    components = spawned_actor.get_components_by_class(unreal.StaticMeshComponent)
    log(f"Found {len(components)} StaticMeshComponent(s)")

    for i, comp in enumerate(components):
        comp_name = comp.get_name()
        rel_loc = comp.get_editor_property('relative_location')
        rel_rot = comp.get_editor_property('relative_rotation')
        mesh = comp.get_editor_property('static_mesh')
        mesh_name = mesh.get_name() if mesh else "NULL"

        log(f"\n  Component {i}: {comp_name}")
        log(f"    Mesh: {mesh_name}")
        log(f"    Relative Location: X={rel_loc.x:.4f}, Y={rel_loc.y:.4f}, Z={rel_loc.z:.4f}")
        log(f"    Relative Rotation: P={rel_rot.pitch:.2f}, Y={rel_rot.yaw:.2f}, R={rel_rot.roll:.2f}")

    # Try to get weapon_mesh property specifically
    log("\n--- WEAPON_MESH PROPERTY ---")
    try:
        weapon_mesh = spawned_actor.get_editor_property('weapon_mesh')
        if weapon_mesh:
            rel_loc = weapon_mesh.get_editor_property('relative_location')
            rel_rot = weapon_mesh.get_editor_property('relative_rotation')
            log(f"weapon_mesh Location: X={rel_loc.x:.4f}, Y={rel_loc.y:.4f}, Z={rel_loc.z:.4f}")
            log(f"weapon_mesh Rotation: P={rel_rot.pitch:.2f}, Y={rel_rot.yaw:.2f}, R={rel_rot.roll:.2f}")
        else:
            log("weapon_mesh is NULL")
    except Exception as e:
        log(f"weapon_mesh error: {e}")

    # Clean up
    spawned_actor.destroy_actor()
    log("\n--- ACTOR DESTROYED ---")

if __name__ == "__main__":
    main()
