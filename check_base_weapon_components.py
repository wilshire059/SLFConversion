#!/usr/bin/env python3
"""Check B_Item_Weapon base class component setup"""

import unreal

def log(msg):
    unreal.log_warning(str(msg))

def check_base_weapon():
    log("=" * 70)
    log("BASE WEAPON (B_Item_Weapon) COMPONENT CHECK")
    log("=" * 70)

    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/B_Item_Weapon"
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)

    if not bp:
        log("ERROR: Could not load B_Item_Weapon")
        return

    gen_class = bp.generated_class()
    if not gen_class:
        log("ERROR: No generated class")
        return

    log(f"Generated class: {gen_class.get_name()}")

    cdo = unreal.get_default_object(gen_class)
    if cdo:
        log(f"CDO: {cdo.get_name()}")

        # Check root component
        root = cdo.get_editor_property("root_component")
        log(f"RootComponent: {root.get_name() if root else 'None'}")

        # Get all scene components
        all_comps = cdo.get_components_by_class(unreal.SceneComponent)
        log(f"\nAll SceneComponents ({len(all_comps)}):")
        for comp in all_comps:
            log(f"  - {comp.get_name()} ({comp.get_class().get_name()})")

            # Get relative transform
            rel_loc = comp.get_editor_property("relative_location")
            rel_rot = comp.get_editor_property("relative_rotation")
            log(f"      Loc: X={rel_loc.x:.2f}, Y={rel_loc.y:.2f}, Z={rel_loc.z:.2f}")
            log(f"      Rot: P={rel_rot.pitch:.2f}, Y={rel_rot.yaw:.2f}, R={rel_rot.roll:.2f}")

            # For StaticMeshComponent, get the mesh
            if comp.get_class().get_name() == "StaticMeshComponent":
                mesh = comp.get_editor_property("static_mesh")
                log(f"      Mesh: {mesh.get_name() if mesh else 'None'}")

        # Try to get SimpleConstructionScript info
        try:
            # The Blueprint object has SCS
            scs = bp.get_editor_property("simple_construction_script")
            if scs:
                log(f"\n SimpleConstructionScript found")
                # Get all nodes
                try:
                    nodes = scs.get_all_nodes()
                    log(f"  SCS Nodes: {len(nodes)}")
                    for node in nodes:
                        log(f"    - {node.get_name()}")
                except Exception as e:
                    log(f"  SCS nodes error: {e}")
        except Exception as e:
            log(f"\nSCS error: {e}")

    # Also spawn an instance and check
    log("\n--- Spawning temporary instance ---")
    world = unreal.EditorLevelLibrary.get_editor_world()
    if world:
        actor = unreal.EditorLevelLibrary.spawn_actor_from_class(gen_class, unreal.Vector(0, 0, 10000))
        if actor:
            log(f"Spawned: {actor.get_name()}")

            all_comps = actor.get_components_by_class(unreal.SceneComponent)
            log(f"Instance SceneComponents ({len(all_comps)}):")
            for comp in all_comps:
                log(f"  - {comp.get_name()} ({comp.get_class().get_name()})")

                # Get relative transform
                rel_loc = comp.get_editor_property("relative_location")
                rel_rot = comp.get_editor_property("relative_rotation")
                log(f"      Loc: X={rel_loc.x:.2f}, Y={rel_loc.y:.2f}, Z={rel_loc.z:.2f}")
                log(f"      Rot: P={rel_rot.pitch:.2f}, Y={rel_rot.yaw:.2f}, R={rel_rot.roll:.2f}")

                if comp.get_class().get_name() == "StaticMeshComponent":
                    mesh = comp.get_editor_property("static_mesh")
                    log(f"      Mesh: {mesh.get_name() if mesh else 'None'}")

            # Destroy the test actor
            actor.destroy_actor()
            log("Test actor destroyed")

    log("\n" + "=" * 70)

if __name__ == "__main__":
    check_base_weapon()
