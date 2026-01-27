# extract_player_transforms.py
# Extract weapon transforms from bp_only backup
# Run this on bp_only project!

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
    log("=" * 70)
    log("EXTRACTING PLAYER WEAPON TRANSFORMS FROM BP_ONLY")
    log("=" * 70)

    results = {}

    for bp_path in PLAYER_WEAPONS:
        bp_name = bp_path.split("/")[-1]
        log(f"\n[{bp_name}]")

        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            log(f"  Could not load")
            continue

        gen_class = bp.generated_class()
        if not gen_class:
            log(f"  No generated class")
            continue

        weapon_data = {"path": bp_path}

        # Spawn actor to get component transforms
        try:
            world = unreal.EditorLevelLibrary.get_editor_world()
            spawn_loc = unreal.Vector(-100000, -100000, -100000)
            actor = unreal.EditorLevelLibrary.spawn_actor_from_class(gen_class, spawn_loc)

            if actor:
                # Look for static mesh component (WeaponMesh)
                mesh_comps = actor.get_components_by_class(unreal.StaticMeshComponent)
                for comp in mesh_comps:
                    comp_name = comp.get_name()
                    rel_loc = comp.get_editor_property("relative_location")
                    rel_rot = comp.get_editor_property("relative_rotation")
                    mesh = comp.get_editor_property("static_mesh")
                    mesh_path = mesh.get_path_name() if mesh else None

                    log(f"  [{comp_name}]")
                    log(f"    Location: ({rel_loc.x:.2f}, {rel_loc.y:.2f}, {rel_loc.z:.2f})")
                    log(f"    Rotation: (Roll={rel_rot.roll:.2f}, Pitch={rel_rot.pitch:.2f}, Yaw={rel_rot.yaw:.2f})")
                    log(f"    Mesh: {mesh_path}")

                    if comp_name.lower() == "weaponmesh" or "weapon" in comp_name.lower():
                        weapon_data["location"] = {"x": rel_loc.x, "y": rel_loc.y, "z": rel_loc.z}
                        weapon_data["rotation"] = {"roll": rel_rot.roll, "pitch": rel_rot.pitch, "yaw": rel_rot.yaw}
                        weapon_data["mesh_path"] = mesh_path

                actor.destroy_actor()
        except Exception as e:
            log(f"  Spawn error: {e}")

        if "location" in weapon_data:
            results[bp_name] = weapon_data

    # Save results
    output_path = "C:/scripts/SLFConversion/migration_cache/player_weapon_transforms.json"
    with open(output_path, "w") as f:
        json.dump(results, f, indent=2)
    log(f"\n\nSaved to: {output_path}")
    log(f"Found {len(results)} weapons with transform data")

if __name__ == "__main__":
    extract_transforms()
