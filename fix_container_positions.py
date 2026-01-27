# fix_container_positions.py
# Fix hovering B_Container instances by replacing them with fresh spawns

import unreal

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("FIXING CONTAINER POSITIONS IN LEVEL")
    unreal.log_warning("=" * 70)

    # Load the Demo level
    level_path = "/Game/SoulslikeFramework/Maps/L_Demo_Showcase"
    unreal.log_warning(f"Loading level: {level_path}")

    try:
        success = unreal.EditorLevelLibrary.load_level(level_path)
        unreal.log_warning(f"Load level result: {success}")
    except Exception as e:
        unreal.log_warning(f"Load level: {e}")

    # Get all actors in the level
    subsystem = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    world = subsystem.get_editor_world() if subsystem else None
    if not world:
        unreal.log_error("No editor world")
        return

    # Find B_Container instances
    actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    all_actors = actor_subsystem.get_all_level_actors() if actor_subsystem else []
    containers = []
    for actor in all_actors:
        class_name = actor.get_class().get_name()
        if "B_Container" in class_name:
            containers.append(actor)

    unreal.log_warning(f"Found {len(containers)} B_Container instances")

    if len(containers) == 0:
        unreal.log_warning("No containers to fix")
        return

    # Load the B_Container class for spawning new instances
    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Container"
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_error(f"Could not load: {bp_path}")
        return

    container_class = bp.generated_class()
    if not container_class:
        unreal.log_error("No generated class")
        return

    # For each container, record its position, delete it, and spawn a new one
    for i, old_actor in enumerate(containers):
        try:
            # Get current transform
            old_transform = old_actor.get_actor_transform()
            old_location = old_transform.translation
            old_rotation = old_transform.rotation.rotator()
            old_scale = old_actor.get_actor_scale3d()

            actor_name = old_actor.get_name()
            unreal.log_warning(f"\n[{i}] {actor_name}")
            unreal.log_warning(f"  Old location: {old_location}")

            # Delete the old actor
            old_actor.destroy_actor()
            unreal.log_warning(f"  Deleted old instance")

            # Spawn a new instance at the same location
            new_actor = unreal.EditorLevelLibrary.spawn_actor_from_class(
                container_class, old_location, old_rotation)

            if new_actor:
                new_actor.set_actor_scale3d(old_scale)
                unreal.log_warning(f"  Spawned new instance: {new_actor.get_name()}")

                # Verify the new position
                new_location = new_actor.get_actor_location()
                unreal.log_warning(f"  New location: {new_location}")
            else:
                unreal.log_error(f"  Failed to spawn new instance!")

        except Exception as e:
            unreal.log_error(f"  Error: {e}")

    # Save the level
    try:
        unreal.EditorLevelLibrary.save_current_level()
        unreal.log_warning("\nLevel saved")
    except Exception as e:
        unreal.log_warning(f"Save level exception: {e}")

    # Also save the asset packages
    try:
        unreal.EditorAssetLibrary.save_loaded_asset(
            unreal.load_asset(level_path))
        unreal.log_warning("Level asset saved via EditorAssetLibrary")
    except Exception as e:
        unreal.log_warning(f"Save asset: {e}")

    unreal.log_warning("=" * 70)
    unreal.log_warning("Done - test containers in PIE")
    unreal.log_warning("=" * 70)

if __name__ == "__main__":
    main()
