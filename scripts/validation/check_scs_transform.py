# check_scs_transform.py
# Check weapon mesh transform at spawn time (before attachment)

import unreal
import json

def log(msg):
    print(f"[SCS] {msg}")
    unreal.log_warning(f"[SCS] {msg}")

WEAPONS = [
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample01",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Katana",
]

def main():
    log("=" * 70)
    log("CHECKING WEAPON MESH TRANSFORMS AT SPAWN")
    log("=" * 70)

    results = {}

    for bp_path in WEAPONS:
        bp_name = bp_path.split('/')[-1]
        log(f"\n--- {bp_name} ---")

        bp = unreal.load_asset(bp_path)
        if not bp:
            log(f"  Could not load")
            continue

        gen_class = bp.generated_class()
        if not gen_class:
            log(f"  No generated class")
            continue

        # Spawn without Instigator (weapon won't attach)
        try:
            world = unreal.EditorLevelLibrary.get_editor_world()
            if not world:
                log(f"  No world")
                continue

            # Spawn at origin
            spawned = unreal.EditorLevelLibrary.spawn_actor_from_class(
                gen_class,
                unreal.Vector(0, 0, 500),
                unreal.Rotator(0, 0, 0)
            )

            if not spawned:
                log(f"  Failed to spawn")
                continue

            log(f"  Spawned: {spawned.get_name()} (Class: {spawned.get_class().get_name()})")

            # Find the StaticMeshComponent
            mesh_comps = spawned.get_components_by_class(unreal.StaticMeshComponent)
            log(f"  Found {len(mesh_comps)} StaticMeshComponent(s)")

            weapon_data = {"class": gen_class.get_name(), "components": []}

            for comp in mesh_comps:
                comp_name = comp.get_name()

                # Get transforms
                rel_loc = comp.get_editor_property('relative_location')
                rel_rot = comp.get_editor_property('relative_rotation')
                rel_scale = comp.get_editor_property('relative_scale3d')

                # Get mesh
                mesh = comp.get_editor_property('static_mesh')
                mesh_name = mesh.get_name() if mesh else "NULL"

                log(f"  Component: {comp_name}")
                log(f"    Mesh: {mesh_name}")
                log(f"    RelativeLoc: X={rel_loc.x:.4f}, Y={rel_loc.y:.4f}, Z={rel_loc.z:.4f}")
                log(f"    RelativeRot: P={rel_rot.pitch:.4f}, Y={rel_rot.yaw:.4f}, R={rel_rot.roll:.4f}")
                log(f"    RelativeScale: X={rel_scale.x:.4f}, Y={rel_scale.y:.4f}, Z={rel_scale.z:.4f}")

                weapon_data["components"].append({
                    "name": comp_name,
                    "mesh": mesh_name,
                    "relative_location": {"x": rel_loc.x, "y": rel_loc.y, "z": rel_loc.z},
                    "relative_rotation": {"pitch": rel_rot.pitch, "yaw": rel_rot.yaw, "roll": rel_rot.roll},
                    "relative_scale": {"x": rel_scale.x, "y": rel_scale.y, "z": rel_scale.z}
                })

            results[bp_name] = weapon_data
            spawned.destroy_actor()

        except Exception as e:
            log(f"  Error: {e}")
            import traceback
            log(traceback.format_exc())

    # Save results
    output_path = "C:/scripts/SLFConversion/migration_cache/scs_transform_check.json"
    with open(output_path, 'w') as f:
        json.dump(results, f, indent=2)
    log(f"\nSaved to: {output_path}")

if __name__ == "__main__":
    main()
