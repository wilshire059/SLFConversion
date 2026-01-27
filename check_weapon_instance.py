#!/usr/bin/env python3
"""Check spawned weapon instance component transforms"""

import unreal

def log(msg):
    unreal.log_warning(str(msg))

WEAPONS = [
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample01",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Shield",
]

def check_weapons():
    log("=" * 70)
    log("SPAWNED WEAPON INSTANCE CHECK")
    log("=" * 70)

    world = unreal.EditorLevelLibrary.get_editor_world()
    if not world:
        log("ERROR: No editor world")
        return

    for bp_path in WEAPONS:
        bp_name = bp_path.split("/")[-1]
        log(f"\n=== {bp_name} ===")

        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            log(f"  ERROR: Could not load Blueprint")
            continue

        gen_class = bp.generated_class()
        if not gen_class:
            log(f"  ERROR: No generated class")
            continue

        # Spawn instance
        actor = unreal.EditorLevelLibrary.spawn_actor_from_class(gen_class, unreal.Vector(0, 0, 10000))
        if not actor:
            log(f"  ERROR: Could not spawn actor")
            continue

        log(f"  Spawned: {actor.get_name()}")

        # Get all scene components
        all_comps = actor.get_components_by_class(unreal.SceneComponent)
        log(f"  SceneComponents ({len(all_comps)}):")

        for comp in all_comps:
            comp_name = comp.get_name()
            comp_class = comp.get_class().get_name()

            # Get world and relative transforms
            world_loc = comp.get_editor_property("relative_location")
            world_rot = comp.get_editor_property("relative_rotation")

            log(f"\n    {comp_name} ({comp_class}):")
            log(f"      RelativeLoc: X={world_loc.x:.2f}, Y={world_loc.y:.2f}, Z={world_loc.z:.2f}")
            log(f"      RelativeRot: P={world_rot.pitch:.2f}, Y={world_rot.yaw:.2f}, R={world_rot.roll:.2f}")

            # For StaticMeshComponent, get the mesh
            if comp_class == "StaticMeshComponent":
                mesh = comp.get_editor_property("static_mesh")
                log(f"      Mesh: {mesh.get_name() if mesh else 'None'}")

                # Get attach parent
                attach_parent = comp.get_attach_parent()
                attach_socket = comp.get_attach_socket_name()
                log(f"      AttachParent: {attach_parent.get_name() if attach_parent else 'None'}")
                log(f"      AttachSocket: {attach_socket}")

        # Destroy the test actor
        actor.destroy_actor()
        log(f"\n  Test actor destroyed")

    log("\n" + "=" * 70)

if __name__ == "__main__":
    check_weapons()
