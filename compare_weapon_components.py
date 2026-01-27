# compare_weapon_components.py
# Compare weapon component details between bp_only and C++ version
# Run this on BOTH projects and compare output

import unreal
import json

def log(msg):
    unreal.log_warning(str(msg))

WEAPONS = [
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Shield",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample01",
]

def compare():
    log("=" * 70)
    log("DETAILED WEAPON COMPONENT COMPARISON")
    log("=" * 70)

    results = {}

    for bp_path in WEAPONS:
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

        weapon_data = {"path": bp_path, "components": []}

        # Spawn actor
        try:
            spawn_loc = unreal.Vector(-100000, -100000, -100000)
            actor = unreal.EditorLevelLibrary.spawn_actor_from_class(gen_class, spawn_loc)

            if actor:
                log(f"  Class: {actor.get_class().get_name()}")
                log(f"  Parent: {actor.get_class().get_class().get_name()}")

                # Get all components
                all_comps = actor.get_components_by_class(unreal.ActorComponent)
                log(f"\n  COMPONENTS ({len(all_comps)}):")

                for comp in all_comps:
                    comp_name = comp.get_name()
                    comp_class = comp.get_class().get_name()
                    log(f"\n    [{comp_name}] ({comp_class})")

                    comp_data = {
                        "name": comp_name,
                        "class": comp_class
                    }

                    # If it's a SceneComponent, get transform
                    if isinstance(comp, unreal.SceneComponent):
                        rel_loc = comp.get_editor_property("relative_location")
                        rel_rot = comp.get_editor_property("relative_rotation")
                        rel_scale = comp.get_editor_property("relative_scale3d")
                        attach_parent = comp.get_attach_parent()
                        parent_name = attach_parent.get_name() if attach_parent else "None"

                        log(f"      AttachParent: {parent_name}")
                        log(f"      Location: ({rel_loc.x:.2f}, {rel_loc.y:.2f}, {rel_loc.z:.2f})")
                        log(f"      Rotation: (P={rel_rot.pitch:.2f}, Y={rel_rot.yaw:.2f}, R={rel_rot.roll:.2f})")
                        log(f"      Scale: ({rel_scale.x:.2f}, {rel_scale.y:.2f}, {rel_scale.z:.2f})")

                        comp_data["attach_parent"] = parent_name
                        comp_data["location"] = f"({rel_loc.x:.2f}, {rel_loc.y:.2f}, {rel_loc.z:.2f})"
                        comp_data["rotation"] = f"(P={rel_rot.pitch:.2f}, Y={rel_rot.yaw:.2f}, R={rel_rot.roll:.2f})"

                    # If it's a StaticMeshComponent, get mesh
                    if isinstance(comp, unreal.StaticMeshComponent):
                        mesh = comp.get_editor_property("static_mesh")
                        mesh_name = mesh.get_name() if mesh else "NONE"
                        visible = comp.is_visible()
                        log(f"      Mesh: {mesh_name}")
                        log(f"      Visible: {visible}")
                        comp_data["mesh"] = mesh_name
                        comp_data["visible"] = visible

                    weapon_data["components"].append(comp_data)

                actor.destroy_actor()

        except Exception as e:
            log(f"  Spawn error: {e}")

        results[bp_name] = weapon_data

    # Save results
    output_path = "C:/scripts/SLFConversion/migration_cache/weapon_component_comparison.json"
    with open(output_path, "w") as f:
        json.dump(results, f, indent=2)
    log(f"\n\nSaved to: {output_path}")

if __name__ == "__main__":
    compare()
