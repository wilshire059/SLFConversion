# extract_player_weapon_transforms.py
# Extract mesh transforms from player weapon Blueprints
# Run on bp_only to get original values

import unreal
import json

def log(msg):
    unreal.log_warning(str(msg))

PLAYER_WEAPONS = [
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample01",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample02",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Greatsword",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Katana",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Shield",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_BossMace",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_PoisonSword",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/B_Item_Weapon",
]

def extract_transforms():
    log("")
    log("=" * 70)
    log("EXTRACTING PLAYER WEAPON MESH TRANSFORMS")
    log("=" * 70)

    results = {}

    for bp_path in PLAYER_WEAPONS:
        bp_name = bp_path.split("/")[-1]
        log(f"\n[{bp_name}]")

        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            log(f"  Could not load Blueprint")
            continue

        gen_class = bp.generated_class()
        if not gen_class:
            log(f"  No generated class")
            continue

        weapon_data = {
            "path": bp_path,
            "mesh_component": None
        }

        # Spawn temp actor to get SCS components
        try:
            world = unreal.EditorLevelLibrary.get_editor_world()
            spawn_loc = unreal.Vector(-100000, -100000, -100000)
            actor = unreal.EditorLevelLibrary.spawn_actor_from_class(gen_class, spawn_loc)

            if actor:
                # Find StaticMeshComponent
                mesh_comps = actor.get_components_by_class(unreal.StaticMeshComponent)
                for comp in mesh_comps:
                    comp_name = comp.get_name()

                    # Get transform
                    rel_loc = comp.get_editor_property("relative_location")
                    rel_rot = comp.get_editor_property("relative_rotation")

                    # Get mesh
                    mesh = comp.get_editor_property("static_mesh")
                    mesh_name = mesh.get_name() if mesh else "NONE"

                    log(f"  Component: {comp_name}")
                    log(f"    Location: ({rel_loc.x:.2f}, {rel_loc.y:.2f}, {rel_loc.z:.2f})")
                    log(f"    Rotation: (P={rel_rot.pitch:.2f}, Y={rel_rot.yaw:.2f}, R={rel_rot.roll:.2f})")
                    log(f"    Mesh: {mesh_name}")

                    weapon_data["mesh_component"] = {
                        "name": comp_name,
                        "location": {"x": rel_loc.x, "y": rel_loc.y, "z": rel_loc.z},
                        "rotation": {"pitch": rel_rot.pitch, "yaw": rel_rot.yaw, "roll": rel_rot.roll},
                        "mesh": mesh_name
                    }

                actor.destroy_actor()
            else:
                log(f"  Failed to spawn actor")
        except Exception as e:
            log(f"  Error: {e}")

        results[bp_name] = weapon_data

    # Save results
    output_path = "C:/scripts/SLFConversion/migration_cache/player_weapon_transforms.json"
    with open(output_path, "w") as f:
        json.dump(results, f, indent=2)
    log(f"\nSaved to: {output_path}")

if __name__ == "__main__":
    extract_transforms()
