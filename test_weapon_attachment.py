# test_weapon_attachment.py
# Test weapon attachment in SLFConversion for comparison with bp_only
# Run in PIE (Play In Editor) mode after equipping a weapon

import unreal
import json
import os

# Output file for comparison
OUTPUT_FILE = "C:/scripts/slfconversion/migration_cache/weapon_attachment_slfconversion.json"

def log(msg):
    print(f"[SLFConversion ATTACH] {msg}")
    unreal.log_warning(f"[SLFConversion ATTACH] {msg}")

def get_vector_dict(v):
    return {"x": round(v.x, 4), "y": round(v.y, 4), "z": round(v.z, 4)}

def get_rotator_dict(r):
    return {"pitch": round(r.pitch, 4), "yaw": round(r.yaw, 4), "roll": round(r.roll, 4)}

def main():
    log("=" * 70)
    log("WEAPON ATTACHMENT TEST - SLFConversion")
    log("=" * 70)

    results = {
        "project": "SLFConversion",
        "weapons": {}
    }

    # Get the PIE world (not editor world during PIE)
    world = None
    try:
        # Try PIE world first (index 0 for first PIE instance)
        world = unreal.EditorLevelLibrary.get_pie_world(0)
        if world:
            log(f"Using PIE world: {world.get_name()}")
    except Exception as e:
        log(f"get_pie_world failed: {e}")

    if not world:
        # Fallback to editor world (for non-PIE)
        try:
            world = unreal.EditorLevelLibrary.get_editor_world()
            if world:
                log(f"Using editor world: {world.get_name()}")
        except Exception as e:
            log(f"get_editor_world failed: {e}")

    if not world:
        log("ERROR: No world available! Run this during PIE or in editor.")
        return

    # Find all weapon actors in the world
    all_actors = unreal.GameplayStatics.get_all_actors_of_class(world, unreal.Actor)

    weapon_actors = []
    for actor in all_actors:
        class_name = actor.get_class().get_name()
        if "B_Item_Weapon" in class_name:
            weapon_actors.append(actor)

    log(f"Found {len(weapon_actors)} weapon actors")

    for weapon in weapon_actors:
        weapon_name = weapon.get_class().get_name()
        log(f"")
        log(f"--- {weapon_name} ---")

        weapon_data = {
            "class": weapon_name,
            "actor_name": weapon.get_name()
        }

        # Actor transform
        actor_loc = weapon.get_actor_location()
        actor_rot = weapon.get_actor_rotation()
        weapon_data["actor_world_location"] = get_vector_dict(actor_loc)
        weapon_data["actor_world_rotation"] = get_rotator_dict(actor_rot)
        log(f"  Actor World: Location={actor_loc}, Rotation={actor_rot}")

        # Root component
        root = weapon.root_component
        if root:
            rel_loc = root.get_editor_property('relative_location')
            rel_rot = root.get_editor_property('relative_rotation')
            world_loc = root.get_world_location()
            world_rot = root.get_world_rotation()

            weapon_data["root_relative_location"] = get_vector_dict(rel_loc)
            weapon_data["root_relative_rotation"] = get_rotator_dict(rel_rot)
            weapon_data["root_world_location"] = get_vector_dict(world_loc)
            weapon_data["root_world_rotation"] = get_rotator_dict(world_rot)

            # Check attachment
            attach_parent = root.get_attach_parent()
            if attach_parent:
                attach_socket = root.get_attach_socket_name()
                weapon_data["attached_to"] = attach_parent.get_name()
                weapon_data["attach_socket"] = str(attach_socket)
                log(f"  Attached to: {attach_parent.get_name()} socket={attach_socket}")

                # Get socket world position
                socket_loc = attach_parent.get_socket_location(attach_socket)
                weapon_data["socket_world_location"] = get_vector_dict(socket_loc)
                log(f"  Socket World: {socket_loc}")
            else:
                weapon_data["attached"] = False
                log(f"  NOT ATTACHED")

        # Find StaticMeshComponent (weapon mesh)
        mesh_comps = weapon.get_components_by_class(unreal.StaticMeshComponent)
        if mesh_comps:
            mesh = mesh_comps[0]
            mesh_rel_loc = mesh.get_editor_property('relative_location')
            mesh_rel_rot = mesh.get_editor_property('relative_rotation')
            mesh_world_loc = mesh.get_world_location()
            mesh_world_rot = mesh.get_world_rotation()

            weapon_data["mesh_component_name"] = mesh.get_name()
            weapon_data["mesh_relative_location"] = get_vector_dict(mesh_rel_loc)
            weapon_data["mesh_relative_rotation"] = get_rotator_dict(mesh_rel_rot)
            weapon_data["mesh_world_location"] = get_vector_dict(mesh_world_loc)
            weapon_data["mesh_world_rotation"] = get_rotator_dict(mesh_world_rot)

            log(f"  Mesh '{mesh.get_name()}':")
            log(f"    Relative: Location={mesh_rel_loc}, Rotation={mesh_rel_rot}")
            log(f"    World: Location={mesh_world_loc}, Rotation={mesh_world_rot}")

            # Calculate offset from socket
            if "socket_world_location" in weapon_data:
                socket_loc = unreal.Vector(
                    weapon_data["socket_world_location"]["x"],
                    weapon_data["socket_world_location"]["y"],
                    weapon_data["socket_world_location"]["z"]
                )
                offset = mesh_world_loc - socket_loc
                offset_magnitude = (offset.x**2 + offset.y**2 + offset.z**2)**0.5
                weapon_data["mesh_offset_from_socket"] = get_vector_dict(offset)
                weapon_data["mesh_offset_magnitude"] = round(offset_magnitude, 4)
                log(f"    Offset from socket: {offset}")
                log(f"    Offset magnitude: {round(offset_magnitude, 4)}")
                log(f"    Expected relative Z: {mesh_rel_loc.z}")

            # Get mesh asset name
            static_mesh = mesh.get_editor_property('static_mesh')
            if static_mesh:
                weapon_data["static_mesh"] = static_mesh.get_name()

        results["weapons"][weapon_name] = weapon_data

    # Save results
    os.makedirs(os.path.dirname(OUTPUT_FILE), exist_ok=True)
    with open(OUTPUT_FILE, 'w') as f:
        json.dump(results, f, indent=2)

    log("")
    log(f"Results saved to: {OUTPUT_FILE}")
    log("Compare with bp_only results!")

if __name__ == "__main__":
    main()
