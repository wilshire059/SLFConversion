# verify_weapon_spawn.py
# Spawn player weapons and verify mesh/transforms are applied

import unreal

def log(msg):
    unreal.log_warning(str(msg))

PLAYER_WEAPONS = [
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample01",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_PoisonSword",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Shield",
]

def verify():
    log("=" * 70)
    log("VERIFYING SPAWNED WEAPON TRANSFORMS")
    log("=" * 70)

    for bp_path in PLAYER_WEAPONS:
        bp_name = bp_path.split("/")[-1]
        log(f"\n[{bp_name}]")

        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            log(f"  ERROR: Could not load Blueprint")
            continue

        gen_class = bp.generated_class()
        if not gen_class:
            log(f"  ERROR: No generated class")
            continue

        # First check CDO values
        try:
            cdo = unreal.get_default_object(gen_class)
            loc = cdo.get_editor_property("default_mesh_relative_location")
            rot = cdo.get_editor_property("default_mesh_relative_rotation")
            mesh = cdo.get_editor_property("default_weapon_mesh")
            mesh_name = mesh.get_name() if mesh else "None"
            log(f"  CDO Location: ({loc.x:.2f}, {loc.y:.2f}, {loc.z:.2f})")
            log(f"  CDO Rotation: (P={rot.pitch:.2f}, Y={rot.yaw:.2f}, R={rot.roll:.2f})")
            log(f"  CDO Mesh: {mesh_name}")
        except Exception as e:
            log(f"  CDO check error: {e}")

        # Spawn actor
        try:
            spawn_loc = unreal.Vector(-100000, -100000, -100000)
            actor = unreal.EditorLevelLibrary.spawn_actor_from_class(gen_class, spawn_loc)

            if actor:
                log(f"  SPAWNED - Checking components:")

                # Check all static mesh components
                mesh_comps = actor.get_components_by_class(unreal.StaticMeshComponent)
                for comp in mesh_comps:
                    comp_name = comp.get_name()
                    rel_loc = comp.get_editor_property("relative_location")
                    rel_rot = comp.get_editor_property("relative_rotation")
                    mesh = comp.get_editor_property("static_mesh")
                    mesh_name = mesh.get_name() if mesh else "NONE"
                    visible = comp.is_visible()

                    log(f"    [{comp_name}]")
                    log(f"      Location: ({rel_loc.x:.2f}, {rel_loc.y:.2f}, {rel_loc.z:.2f})")
                    log(f"      Rotation: (P={rel_rot.pitch:.2f}, Y={rel_rot.yaw:.2f}, R={rel_rot.roll:.2f})")
                    log(f"      Mesh: {mesh_name}")
                    log(f"      Visible: {visible}")

                actor.destroy_actor()
        except Exception as e:
            log(f"  Spawn error: {e}")

if __name__ == "__main__":
    verify()
