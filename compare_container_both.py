# compare_container_both.py
# Compare B_Container properties and components

import unreal

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("COMPARING B_Container - Current Project")
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

    # Get CDO
    cdo = unreal.get_default_object(gen_class)
    unreal.log_warning(f"\nCDO: {cdo.get_name()}")

    # Check parent class
    try:
        parent = bp.get_editor_property("parent_class")
        unreal.log_warning(f"Parent class: {parent.get_name() if parent else 'None'}")
    except:
        unreal.log_warning("Parent class: Could not determine")

    # Check key properties
    unreal.log_warning("\n--- CDO Properties ---")
    props_to_check = [
        "open_montage", "open_vfx", "move_distance", "speed_multiplier",
        "cached_intensity", "chest_box_mesh", "interaction_text",
        "lid_open_angle", "b_is_open"
    ]
    for prop in props_to_check:
        try:
            val = cdo.get_editor_property(prop)
            unreal.log_warning(f"  {prop}: {val}")
        except Exception as e:
            unreal.log_warning(f"  {prop}: NOT FOUND")

    # Spawn instance and check components
    unreal.log_warning("\n--- Spawning Instance ---")
    spawn_loc = unreal.Vector(0, 0, 1000)
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
                unreal.log_warning(f"\n  [{comp_name}] ({comp_class})")

                if isinstance(comp, unreal.SceneComponent):
                    try:
                        rel_loc = comp.get_editor_property("relative_location")
                        rel_rot = comp.get_editor_property("relative_rotation")
                        rel_scale = comp.get_editor_property("relative_scale3d")
                        unreal.log_warning(f"    Loc: X={rel_loc.x:.1f}, Y={rel_loc.y:.1f}, Z={rel_loc.z:.1f}")
                        unreal.log_warning(f"    Rot: P={rel_rot.pitch:.1f}, Y={rel_rot.yaw:.1f}, R={rel_rot.roll:.1f}")
                        unreal.log_warning(f"    Scale: {rel_scale.x:.2f}, {rel_scale.y:.2f}, {rel_scale.z:.2f}")

                        parent = comp.get_attach_parent()
                        if parent:
                            unreal.log_warning(f"    Parent: {parent.get_name()}")
                    except:
                        pass

                # Check mesh for StaticMeshComponents
                if isinstance(comp, unreal.StaticMeshComponent):
                    try:
                        mesh = comp.get_editor_property("static_mesh")
                        unreal.log_warning(f"    Mesh: {mesh.get_name() if mesh else 'NONE'}")
                    except:
                        unreal.log_warning(f"    Mesh: ERROR")

            # Destroy the temp actor
            actor.destroy_actor()
            unreal.log_warning("\nDestroyed temp actor")
    except Exception as e:
        unreal.log_error(f"Spawn error: {e}")

    unreal.log_warning("\n" + "=" * 70)
    unreal.log_warning("Done")
    unreal.log_warning("=" * 70)

if __name__ == "__main__":
    main()
