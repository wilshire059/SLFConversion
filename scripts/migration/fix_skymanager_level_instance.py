"""
Fix B_SkyManager Level Instance

Problem: After reparenting B_SkyManager Blueprint to C++ class,
the level instance has stale component references (0 components).

Solution: Delete the old instance and spawn a new one at the same location.
The new instance will inherit from the updated Blueprint CDO with all C++ components.

USAGE:
1. Run in editor: Execute from the Output Log Python console
   OR
2. Run headless: UnrealEditor-Cmd.exe ... -run=pythonscript -script="this_script.py"
   (headless mode will attempt to load the level)
"""

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/skymanager_fix.txt"

def log(msg):
    """Log to file and UE"""
    unreal.log(msg)
    with open(OUTPUT_FILE, "a", encoding="utf-8") as f:
        f.write(msg + "\n")

def get_world():
    """Try multiple approaches to get the editor world"""
    # Try 1: EditorActorSubsystem
    try:
        actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
        if actor_subsystem:
            world = actor_subsystem.get_world()
            if world:
                return world
    except:
        pass

    # Try 2: UnrealEditorSubsystem
    try:
        editor_subsystem = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
        if editor_subsystem:
            world = editor_subsystem.get_editor_world()
            if world:
                return world
    except:
        pass

    # Try 3: LevelEditorSubsystem load
    try:
        level_subsystem = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
        level_path = "/Game/SoulslikeFramework/Maps/L_Demo_Showcase"
        if level_subsystem:
            level_subsystem.load_level(level_path)
            editor_subsystem = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
            if editor_subsystem:
                return editor_subsystem.get_editor_world()
    except:
        pass

    return None

def fix_skymanager_instance():
    # Clear previous output
    with open(OUTPUT_FILE, "w", encoding="utf-8") as f:
        f.write("")

    log("=" * 60)
    log("Fixing B_SkyManager Level Instance")
    log("=" * 60)

    # Load the Blueprint class
    bp_path = "/Game/SoulslikeFramework/Blueprints/Sky/B_SkyManager"
    log(f"Loading Blueprint: {bp_path}")
    bp_asset = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp_asset:
        log(f"ERROR: Could not load Blueprint at {bp_path}")
        return False

    gen_class = bp_asset.generated_class()
    if not gen_class:
        log("ERROR: Blueprint has no generated class")
        return False

    log(f"Blueprint class: {gen_class.get_name()}")

    # Check CDO components first
    cdo = unreal.get_default_object(gen_class)
    cdo_comps = cdo.get_components_by_class(unreal.ActorComponent)
    log(f"CDO has {len(cdo_comps)} components")
    for comp in cdo_comps:
        log(f"  - {comp.get_name()}")

    # Get world
    world = get_world()
    if not world:
        log("")
        log("=" * 60)
        log("COULD NOT ACCESS EDITOR WORLD")
        log("=" * 60)
        log("")
        log("This script must be run with the editor open.")
        log("In headless mode, the level cannot be modified.")
        log("")
        log("MANUAL FIX INSTRUCTIONS:")
        log("-" * 40)
        log("1. Open Unreal Editor")
        log("2. Open level: L_Demo_Showcase")
        log("3. In the World Outliner, find any actor with 'SkyManager' in name")
        log("4. Delete that actor")
        log("5. From Content Browser, drag B_SkyManager into the level")
        log("   Path: Content/SoulslikeFramework/Blueprints/Sky/B_SkyManager")
        log("6. Position it at origin (0, 0, 0)")
        log("7. Save the level (Ctrl+S)")
        log("")
        log("The new instance will inherit all C++ components.")
        log("")
        log(f"Results written to: {OUTPUT_FILE}")
        return False

    log(f"Got world: {world.get_name()}")

    # Find existing B_SkyManager instances
    log("Searching for existing B_SkyManager instances...")
    all_actors = unreal.GameplayStatics.get_all_actors_of_class(world, unreal.Actor)
    log(f"Total actors in world: {len(all_actors)}")

    skymanager_instances = []
    for actor in all_actors:
        actor_class = actor.get_class()
        class_name = actor_class.get_name()
        if "SkyManager" in class_name:
            skymanager_instances.append(actor)
            log(f"  Found: {actor.get_name()} at {actor.get_actor_location()}")

    if not skymanager_instances:
        log("")
        log("No existing B_SkyManager instances found.")
        log("Spawning new instance at origin...")

        spawn_location = unreal.Vector(0, 0, 0)
        spawn_rotation = unreal.Rotator(0, 0, 0)

        new_actor = unreal.EditorLevelLibrary.spawn_actor_from_class(
            gen_class, spawn_location, spawn_rotation
        )

        if new_actor:
            log(f"Spawned new B_SkyManager: {new_actor.get_name()}")
            comps = new_actor.get_components_by_class(unreal.ActorComponent)
            log(f"  Components: {len(comps)}")
            for comp in comps:
                log(f"    - {comp.get_name()}")
        else:
            log("ERROR: Failed to spawn new instance")
            return False

    else:
        # Process each existing instance
        for old_actor in skymanager_instances:
            log(f"")
            log(f"Processing: {old_actor.get_name()}")

            old_comps = old_actor.get_components_by_class(unreal.ActorComponent)
            log(f"  Current components: {len(old_comps)}")

            if len(old_comps) >= 7:
                log("  Instance already has components. Skipping.")
                continue

            # Record transform
            old_location = old_actor.get_actor_location()
            old_rotation = old_actor.get_actor_rotation()
            old_scale = old_actor.get_actor_scale3d()
            old_label = old_actor.get_actor_label()

            log(f"  Location: {old_location}")
            log(f"  Rotation: {old_rotation}")
            log(f"  Scale: {old_scale}")
            log(f"  Label: {old_label}")

            # Delete old actor
            log("  Deleting old instance...")
            old_actor.destroy_actor()

            # Spawn new actor at same location
            log("  Spawning new instance...")
            new_actor = unreal.EditorLevelLibrary.spawn_actor_from_class(
                gen_class, old_location, old_rotation
            )

            if new_actor:
                new_actor.set_actor_scale3d(old_scale)
                if old_label:
                    new_actor.set_actor_label(old_label)

                new_comps = new_actor.get_components_by_class(unreal.ActorComponent)
                log(f"  New instance components: {len(new_comps)}")

                for comp in new_comps:
                    log(f"    - {comp.get_name()}")

                if len(new_comps) >= 7:
                    log("  SUCCESS: New instance has all components!")
                else:
                    log("  WARNING: New instance may be missing components")
            else:
                log("  ERROR: Failed to spawn new instance!")
                return False

    # Save the level
    log("")
    log("--- Saving Level ---")
    level_subsystem = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    if level_subsystem:
        success = level_subsystem.save_current_level()
        if success:
            log("Level saved successfully!")
        else:
            log("WARNING: Could not auto-save level")
            log("Please save the level manually (Ctrl+S)")

    log("")
    log("=" * 60)
    log("FIX COMPLETE")
    log("=" * 60)
    log("")
    log("Next steps:")
    log("1. Verify the level was saved")
    log("2. Run PIE test")
    log("3. Rest at resting point and change time")
    log("4. Verify sky lighting changes")
    log("")
    log(f"Results written to: {OUTPUT_FILE}")

    return True

if __name__ == "__main__":
    fix_skymanager_instance()
