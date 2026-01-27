# check_container_components.py
# Check what components exist on B_Container

import unreal

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("CHECKING B_Container COMPONENTS")
    unreal.log_warning("=" * 70)

    # Load the Blueprint
    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Container"
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_error(f"Could not load: {bp_path}")
        return

    gen_class = bp.generated_class()
    if not gen_class:
        unreal.log_error("No generated class")
        return

    # Spawn instance
    unreal.log_warning("\n--- Spawning Instance ---")
    spawn_loc = unreal.Vector(0, 0, 5000)
    spawn_rot = unreal.Rotator(0, 0, 0)

    try:
        actor = unreal.EditorLevelLibrary.spawn_actor_from_class(gen_class, spawn_loc, spawn_rot)
        if actor:
            unreal.log_warning(f"Spawned: {actor.get_name()}")

            # Get all components
            components = actor.get_components_by_class(unreal.ActorComponent)
            unreal.log_warning(f"\nTotal components: {len(components)}")

            for comp in components:
                comp_name = comp.get_name()
                comp_class = comp.get_class().get_name()
                unreal.log_warning(f"  [{comp_name}] ({comp_class})")

                # Check for loot manager specifically
                if "loot" in comp_name.lower() or "loot" in comp_class.lower():
                    unreal.log_warning(f"    *** LOOT MANAGER FOUND ***")

            # Also check via SCS (SimpleConstructionScript)
            unreal.log_warning("\n--- Blueprint SCS Components ---")
            try:
                scs = bp.get_editor_property("simple_construction_script")
                if scs:
                    root_nodes = scs.get_all_nodes()
                    unreal.log_warning(f"SCS nodes: {len(root_nodes)}")
                    for node in root_nodes:
                        template = node.get_editor_property("component_template")
                        if template:
                            unreal.log_warning(f"  SCS: [{template.get_name()}] ({template.get_class().get_name()})")
            except Exception as e:
                unreal.log_warning(f"SCS check error: {e}")

            actor.destroy_actor()
    except Exception as e:
        unreal.log_error(f"Spawn error: {e}")

    unreal.log_warning("\n" + "=" * 70)
    unreal.log_warning("Done")
    unreal.log_warning("=" * 70)

if __name__ == "__main__":
    main()
