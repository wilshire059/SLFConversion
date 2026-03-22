# spawn_bponly_container.py
# Spawn a B_Container instance and inspect its components

import unreal

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("SPAWNING B_Container TO INSPECT COMPONENTS")
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

    unreal.log_warning(f"Class: {gen_class.get_name()}")

    # Spawn an instance
    world = unreal.EditorLevelLibrary.get_editor_world()
    if not world:
        # Create a temp world
        unreal.log_warning("No editor world, using spawn_actor_from_class")

    spawn_loc = unreal.Vector(0, 0, 0)
    spawn_rot = unreal.Rotator(0, 0, 0)

    try:
        actor = unreal.EditorLevelLibrary.spawn_actor_from_class(gen_class, spawn_loc, spawn_rot)
        if actor:
            unreal.log_warning(f"\nSpawned: {actor.get_name()}")

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
                        unreal.log_warning(f"    RelLoc: X={rel_loc.x:.2f}, Y={rel_loc.y:.2f}, Z={rel_loc.z:.2f}")
                        unreal.log_warning(f"    RelRot: P={rel_rot.pitch:.2f}, Y={rel_rot.yaw:.2f}, R={rel_rot.roll:.2f}")
                        unreal.log_warning(f"    Scale: X={rel_scale.x:.2f}, Y={rel_scale.y:.2f}, Z={rel_scale.z:.2f}")

                        # Get parent attachment
                        parent = comp.get_attach_parent()
                        if parent:
                            unreal.log_warning(f"    Parent: {parent.get_name()}")
                        else:
                            unreal.log_warning(f"    Parent: ROOT (no parent)")
                    except Exception as e:
                        unreal.log_warning(f"    Transform error: {e}")

                # Check if it's a mesh component
                if isinstance(comp, unreal.StaticMeshComponent):
                    try:
                        mesh = comp.get_editor_property("static_mesh")
                        unreal.log_warning(f"    StaticMesh: {mesh.get_name() if mesh else 'None'}")
                    except:
                        pass

            # Destroy the temp actor
            actor.destroy_actor()
            unreal.log_warning("\nDestroyed temp actor")
        else:
            unreal.log_error("Failed to spawn actor")
    except Exception as e:
        unreal.log_error(f"Spawn error: {e}")

    unreal.log_warning("\n" + "=" * 70)
    unreal.log_warning("Done")
    unreal.log_warning("=" * 70)

if __name__ == "__main__":
    main()
