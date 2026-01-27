# extract_bp_only_mesh_v2.py
# Extract actual mesh component transforms from bp_only by spawning actors

import unreal
import json

WEAPONS = [
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample01",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Greatsword",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Katana",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Shield",
]

def log(msg):
    print(f"[Extract] {msg}")
    unreal.log_warning(f"[Extract] {msg}")

def main():
    log("=" * 70)
    log("EXTRACTING BP_ONLY MESH TRANSFORMS BY SPAWNING")
    log("=" * 70)

    results = {}

    for bp_path in WEAPONS:
        bp_name = bp_path.split('/')[-1]
        log(f"\n--- {bp_name} ---")

        bp = unreal.load_asset(bp_path)
        if not bp:
            log(f"  Could not load: {bp_path}")
            continue

        gen_class = bp.generated_class()
        if not gen_class:
            log(f"  No generated class")
            continue

        results[bp_name] = {"path": bp_path}

        # Get CDO
        cdo = unreal.get_default_object(gen_class)
        if not cdo:
            log(f"  No CDO")
            continue

        # Find StaticMeshComponent on CDO
        components = cdo.get_components_by_class(unreal.StaticMeshComponent)
        log(f"  CDO has {len(components)} StaticMeshComponent(s)")

        for comp in components:
            comp_name = comp.get_name()
            loc = comp.get_editor_property('relative_location')
            rot = comp.get_editor_property('relative_rotation')
            mesh = comp.get_editor_property('static_mesh')
            mesh_name = mesh.get_name() if mesh else "NULL"

            log(f"  Component: {comp_name}")
            log(f"    Mesh: {mesh_name}")
            log(f"    Location: X={loc.x:.4f}, Y={loc.y:.4f}, Z={loc.z:.4f}")
            log(f"    Rotation: P={rot.pitch:.2f}, Y={rot.yaw:.2f}, R={rot.roll:.2f}")

            results[bp_name][f"cdo_component_{comp_name}"] = {
                "mesh": mesh_name,
                "location": {"x": loc.x, "y": loc.y, "z": loc.z},
                "rotation": {"pitch": rot.pitch, "yaw": rot.yaw, "roll": rot.roll}
            }

        # Try spawning to check runtime values
        try:
            world = unreal.EditorLevelLibrary.get_editor_world()
            if world:
                spawned = unreal.EditorLevelLibrary.spawn_actor_from_class(
                    gen_class,
                    unreal.Vector(0, 0, 1000),
                    unreal.Rotator(0, 0, 0)
                )
                if spawned:
                    runtime_comps = spawned.get_components_by_class(unreal.StaticMeshComponent)
                    log(f"  Spawned actor has {len(runtime_comps)} StaticMeshComponent(s)")

                    for comp in runtime_comps:
                        comp_name = comp.get_name()
                        loc = comp.get_editor_property('relative_location')
                        rot = comp.get_editor_property('relative_rotation')

                        log(f"  Runtime Component: {comp_name}")
                        log(f"    Runtime Location: X={loc.x:.4f}, Y={loc.y:.4f}, Z={loc.z:.4f}")
                        log(f"    Runtime Rotation: P={rot.pitch:.2f}, Y={rot.yaw:.2f}, R={rot.roll:.2f}")

                        results[bp_name][f"runtime_component_{comp_name}"] = {
                            "location": {"x": loc.x, "y": loc.y, "z": loc.z},
                            "rotation": {"pitch": rot.pitch, "yaw": rot.yaw, "roll": rot.roll}
                        }

                    spawned.destroy_actor()
        except Exception as e:
            log(f"  Spawn error: {e}")

    # Save results
    output_path = "C:/scripts/SLFConversion/migration_cache/bp_only_runtime_transforms.json"
    with open(output_path, 'w') as f:
        json.dump(results, f, indent=2)
    log(f"\nSaved to: {output_path}")

if __name__ == "__main__":
    main()
