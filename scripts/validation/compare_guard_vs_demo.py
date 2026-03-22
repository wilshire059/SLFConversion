# compare_guard_vs_demo.py
# Direct comparison of Guard vs Demo enemy weapon setups
# Run on BOTH bp_only and SLFConversion to find the difference

import unreal
import json

def log(msg):
    unreal.log_warning(str(msg))

ENEMIES_TO_CHECK = [
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Guard",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Showcase",
]

def get_component_details(component):
    """Get all relevant details from a component"""
    details = {
        "name": component.get_name(),
        "class": component.get_class().get_name(),
    }

    # Get transform
    try:
        rel_loc = component.get_editor_property("relative_location")
        details["relative_location"] = f"({rel_loc.x:.2f}, {rel_loc.y:.2f}, {rel_loc.z:.2f})"
    except:
        pass

    try:
        rel_rot = component.get_editor_property("relative_rotation")
        details["relative_rotation"] = f"(P={rel_rot.pitch:.2f}, Y={rel_rot.yaw:.2f}, R={rel_rot.roll:.2f})"
    except:
        pass

    # For ChildActorComponent, get the child actor class
    if "ChildActor" in component.get_class().get_name():
        try:
            child_class = component.get_editor_property("child_actor_class")
            if child_class:
                details["child_actor_class"] = child_class.get_name()
                details["child_actor_path"] = child_class.get_path_name()
        except Exception as e:
            details["child_actor_error"] = str(e)

    # For StaticMeshComponent, get the mesh
    if "StaticMesh" in component.get_class().get_name():
        try:
            mesh = component.get_editor_property("static_mesh")
            if mesh:
                details["static_mesh"] = mesh.get_name()
        except:
            details["static_mesh"] = "NONE"

    return details

def extract_enemy_weapon_setup():
    log("")
    log("=" * 80)
    log("GUARD vs DEMO ENEMY WEAPON COMPARISON")
    log("=" * 80)

    results = {}

    for enemy_path in ENEMIES_TO_CHECK:
        enemy_name = enemy_path.split("/")[-1]
        log(f"\n[{enemy_name}]")

        bp = unreal.EditorAssetLibrary.load_asset(enemy_path)
        if not bp:
            log(f"  Could not load Blueprint")
            continue

        gen_class = bp.generated_class()
        if not gen_class:
            log(f"  No generated class")
            continue

        # Get parent class chain
        parent_chain = []
        current = gen_class
        while current:
            parent_chain.append(current.get_name())
            try:
                current = unreal.SystemLibrary.get_super_class(current)
            except:
                break

        log(f"  Parent chain: {' -> '.join(parent_chain[:5])}")

        enemy_data = {
            "path": enemy_path,
            "parent_chain": parent_chain[:5],
            "components": [],
            "weapon_components": []
        }

        # Spawn temp actor to get components
        try:
            world = unreal.EditorLevelLibrary.get_editor_world()
            spawn_loc = unreal.Vector(-100000, -100000, -100000)
            actor = unreal.EditorLevelLibrary.spawn_actor_from_class(gen_class, spawn_loc)

            if actor:
                components = actor.get_components_by_class(unreal.ActorComponent)
                log(f"  Total components: {len(components)}")

                for comp in components:
                    comp_name = comp.get_name()
                    comp_class = comp.get_class().get_name()

                    # Look for weapon-related components
                    if "Weapon" in comp_name or "ChildActor" in comp_class:
                        details = get_component_details(comp)
                        enemy_data["weapon_components"].append(details)
                        log(f"  WEAPON: {comp_name} ({comp_class})")
                        for k, v in details.items():
                            if k not in ["name", "class"]:
                                log(f"    {k}: {v}")

                        # If it's a ChildActorComponent, try to get the child actor
                        if "ChildActor" in comp_class:
                            try:
                                child_actor = comp.get_editor_property("child_actor")
                                if child_actor:
                                    log(f"    [CHILD ACTOR INSTANCE]")
                                    child_comps = child_actor.get_components_by_class(unreal.ActorComponent)
                                    for cc in child_comps:
                                        cc_details = get_component_details(cc)
                                        log(f"      {cc_details.get('name')}: {cc_details}")
                                        if "Mesh" in cc.get_class().get_name():
                                            enemy_data["weapon_components"].append({
                                                "parent": comp_name,
                                                "child_component": cc_details
                                            })
                            except Exception as e:
                                log(f"    Child actor error: {e}")

                # Destroy temp actor
                actor.destroy_actor()
            else:
                log(f"  Failed to spawn actor")
        except Exception as e:
            log(f"  Spawn error: {e}")

        results[enemy_name] = enemy_data

    # Save results
    output_path = "C:/scripts/SLFConversion/migration_cache/guard_vs_demo_comparison.json"
    with open(output_path, "w") as f:
        json.dump(results, f, indent=2, default=str)
    log(f"\nSaved to: {output_path}")

    log("\n" + "=" * 80)

if __name__ == "__main__":
    extract_enemy_weapon_setup()
