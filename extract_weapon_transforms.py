#!/usr/bin/env python3
"""Extract weapon transforms from bp_only project"""

import unreal
import json
import os

def log(msg):
    unreal.log_warning(str(msg))

WEAPON_PATHS = [
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/B_Item_Weapon",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Greatsword",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Katana",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_PoisonSword",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Shield",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample01",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample02",
]

def extract_weapon(bp_path):
    """Extract weapon transform data from Blueprint"""
    log(f"\n=== {bp_path.split('/')[-1]} ===")

    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        log(f"ERROR: Could not load {bp_path}")
        return None

    gen_class = bp.generated_class()
    if not gen_class:
        log("No generated class")
        return None

    result = {
        "path": bp_path,
        "name": bp_path.split("/")[-1],
        "class": gen_class.get_name(),
        "static_mesh_transform": None,
        "static_mesh": None
    }

    # Spawn instance to get runtime component transforms
    try:
        world = unreal.EditorLevelLibrary.get_editor_world()
        if not world:
            log("No editor world")
            return result

        actor = unreal.EditorLevelLibrary.spawn_actor_from_class(gen_class, unreal.Vector(0, 0, 30000))
        if actor:
            # Find StaticMeshComponent
            mesh_comps = actor.get_components_by_class(unreal.StaticMeshComponent)
            for comp in mesh_comps:
                comp_name = comp.get_name()
                loc = comp.get_editor_property("relative_location")
                rot = comp.get_editor_property("relative_rotation")

                mesh = comp.get_editor_property("static_mesh")
                mesh_name = mesh.get_name() if mesh else "None"

                log(f"  {comp_name}: Mesh={mesh_name}")
                log(f"    Location: X={loc.x:.6f}, Y={loc.y:.6f}, Z={loc.z:.6f}")
                log(f"    Rotation: P={rot.pitch:.6f}, Y={rot.yaw:.6f}, R={rot.roll:.6f}")

                # Save first static mesh component with mesh (likely the weapon mesh)
                if mesh and result["static_mesh_transform"] is None:
                    result["static_mesh"] = mesh_name
                    result["static_mesh_transform"] = {
                        "component_name": comp_name,
                        "location": {"x": loc.x, "y": loc.y, "z": loc.z},
                        "rotation": {"pitch": rot.pitch, "yaw": rot.yaw, "roll": rot.roll}
                    }

            actor.destroy_actor()
    except Exception as e:
        log(f"Spawn error: {e}")

    return result

def main():
    log("=" * 70)
    log("WEAPON TRANSFORM EXTRACTION")
    log("=" * 70)

    results = {}

    for path in WEAPON_PATHS:
        result = extract_weapon(path)
        if result:
            results[result["name"]] = result

    # Save results
    output_path = "C:/scripts/slfconversion/migration_cache/weapon_transforms_bp_only.json"
    os.makedirs(os.path.dirname(output_path), exist_ok=True)
    with open(output_path, 'w') as f:
        json.dump(results, f, indent=2)

    log(f"\n\nSaved to: {output_path}")
    log("\n" + "=" * 70)

if __name__ == "__main__":
    main()
