# extract_bp_only_transforms.py
# Extract weapon component transforms from bp_only for comparison
# Run on bp_only project

import unreal
import json

def log(msg):
    unreal.log_warning(str(msg))

WEAPON_BPS = [
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample01",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Greatsword",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Katana",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Shield",
]

def extract_transforms():
    log("=" * 70)
    log("EXTRACTING BP_ONLY WEAPON TRANSFORMS")
    log("=" * 70)

    results = {}

    for bp_path in WEAPON_BPS:
        bp_name = bp_path.split("/")[-1]
        log(f"\n{'='*60}")
        log(f"[{bp_name}]")
        log(f"{'='*60}")

        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            log(f"  Could not load")
            continue

        gen_class = bp.generated_class()
        if not gen_class:
            log(f"  No generated class")
            continue

        # Spawn actor to get component transforms
        try:
            spawn_loc = unreal.Vector(-100000, -100000, -100000)
            actor = unreal.EditorLevelLibrary.spawn_actor_from_class(gen_class, spawn_loc)

            if actor:
                weapon_data = {"class": gen_class.get_name()}

                # Get all components
                components = actor.get_components_by_class(unreal.StaticMeshComponent)
                log(f"  StaticMeshComponents: {len(components)}")

                for comp in components:
                    comp_name = comp.get_name()
                    rel_loc = comp.get_editor_property("relative_location")
                    rel_rot = comp.get_editor_property("relative_rotation")
                    mesh = comp.get_editor_property("static_mesh")
                    mesh_name = mesh.get_name() if mesh else "NONE"

                    log(f"\n  [{comp_name}]")
                    log(f"    Mesh: {mesh_name}")
                    log(f"    Location: ({rel_loc.x:.3f}, {rel_loc.y:.3f}, {rel_loc.z:.3f})")
                    log(f"    Rotation: (P={rel_rot.pitch:.3f}, Y={rel_rot.yaw:.3f}, R={rel_rot.roll:.3f})")

                    weapon_data[comp_name] = {
                        "mesh": mesh_name,
                        "location": {"x": rel_loc.x, "y": rel_loc.y, "z": rel_loc.z},
                        "rotation": {"pitch": rel_rot.pitch, "yaw": rel_rot.yaw, "roll": rel_rot.roll}
                    }

                results[bp_name] = weapon_data
                actor.destroy_actor()

        except Exception as e:
            log(f"  Error: {e}")

    # Save results
    output_path = "C:/scripts/SLFConversion/migration_cache/bp_only_weapon_transforms.json"
    with open(output_path, "w") as f:
        json.dump(results, f, indent=2)
    log(f"\n\nSaved to: {output_path}")

if __name__ == "__main__":
    extract_transforms()
