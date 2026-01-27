"""
Fix B_SkyManager in L_Demo_Showcase Level

This script:
1. Opens the actual L_Demo_Showcase level
2. Finds and deletes any existing B_SkyManager instances
3. Spawns a new B_SkyManager instance
4. Saves the level
"""

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/skymanager_level_fix.txt"

def log(msg):
    """Log to file and UE"""
    unreal.log(msg)
    with open(OUTPUT_FILE, "a", encoding="utf-8") as f:
        f.write(msg + "\n")

def fix_skymanager_in_level():
    # Clear previous output
    with open(OUTPUT_FILE, "w", encoding="utf-8") as f:
        f.write("")

    log("=" * 60)
    log("Fixing B_SkyManager in L_Demo_Showcase")
    log("=" * 60)

    level_path = "/Game/SoulslikeFramework/Maps/L_Demo_Showcase"
    bp_path = "/Game/SoulslikeFramework/Blueprints/Sky/B_SkyManager"

    # Load the Blueprint class first
    log(f"Loading Blueprint: {bp_path}")
    bp_asset = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp_asset:
        log(f"ERROR: Could not load Blueprint")
        return False

    gen_class = bp_asset.generated_class()
    if not gen_class:
        log("ERROR: Blueprint has no generated class")
        return False

    log(f"Blueprint class: {gen_class.get_name()}")

    # Check CDO
    cdo = unreal.get_default_object(gen_class)
    cdo_comps = cdo.get_components_by_class(unreal.ActorComponent)
    log(f"CDO has {len(cdo_comps)} components")

    # Use EditorLoadingAndSavingUtils to open the level
    log("")
    log(f"Opening level: {level_path}")

    # Try to open the map
    try:
        loaded_world = unreal.EditorLoadingAndSavingUtils.load_map(level_path)
        if loaded_world:
            log(f"Loaded world: {loaded_world.get_name()}")
        else:
            log("WARNING: load_map returned None, trying alternative approach")
    except Exception as e:
        log(f"ERROR loading map: {e}")
        loaded_world = None

    # Get the current world after loading
    editor_subsystem = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    world = editor_subsystem.get_editor_world() if editor_subsystem else None

    if not world:
        # Try EditorActorSubsystem
        actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
        world = actor_subsystem.get_world() if actor_subsystem else None

    if not world:
        log("")
        log("ERROR: Could not get world after loading level")
        log("")
        log("MANUAL FIX REQUIRED:")
        log("-" * 40)
        log("1. Open Unreal Editor GUI")
        log("2. File > Open Level > L_Demo_Showcase")
        log("3. In World Outliner, search for 'SkyManager'")
        log("4. Delete any B_SkyManager actor found")
        log("5. From Content Browser: Content/SoulslikeFramework/Blueprints/Sky/B_SkyManager")
        log("6. Drag into the level viewport")
        log("7. Save the level (Ctrl+S)")
        return False

    log(f"Working in world: {world.get_name()}")

    # Find existing B_SkyManager instances
    log("")
    log("Searching for existing B_SkyManager instances...")
    all_actors = unreal.GameplayStatics.get_all_actors_of_class(world, unreal.Actor)
    log(f"Total actors in world: {len(all_actors)}")

    # Find and remove B_SkyManager instances
    skymanagers = []
    for actor in all_actors:
        class_name = actor.get_class().get_name()
        if "SkyManager" in class_name or "B_SkyManager" in class_name:
            skymanagers.append(actor)
            log(f"  Found: {actor.get_name()} at {actor.get_actor_location()}")
            comps = actor.get_components_by_class(unreal.ActorComponent)
            log(f"    Components: {len(comps)}")

    spawn_location = unreal.Vector(0, 0, 0)
    spawn_rotation = unreal.Rotator(0, 0, 0)

    need_spawn = True

    if skymanagers:
        for old_actor in skymanagers:
            spawn_location = old_actor.get_actor_location()
            spawn_rotation = old_actor.get_actor_rotation()
            old_comps = old_actor.get_components_by_class(unreal.ActorComponent)

            if len(old_comps) >= 7:
                log(f"  {old_actor.get_name()} already has {len(old_comps)} components. No fix needed.")
                need_spawn = False
                continue

            log(f"  Deleting {old_actor.get_name()} (stale instance)...")
            old_actor.destroy_actor()
    else:
        log("No existing B_SkyManager found. Will spawn new one.")

    if need_spawn:
        # Spawn new instance only if needed
        log("")
        log("Spawning new B_SkyManager instance...")
        new_actor = unreal.EditorLevelLibrary.spawn_actor_from_class(
            gen_class, spawn_location, spawn_rotation
        )

        if new_actor:
            log(f"Spawned: {new_actor.get_name()}")
            new_comps = new_actor.get_components_by_class(unreal.ActorComponent)
            log(f"  Components: {len(new_comps)}")
            for comp in new_comps:
                log(f"    - {comp.get_name()}")
        else:
            log("ERROR: Failed to spawn new instance")
            return False
    else:
        log("")
        log("Existing B_SkyManager instance is valid. No changes needed.")

    # Save the level
    log("")
    log("Saving level...")

    try:
        saved = unreal.EditorLoadingAndSavingUtils.save_map(world, level_path)
        if saved:
            log("Level saved successfully!")
        else:
            log("WARNING: save_map returned False")
    except Exception as e:
        log(f"ERROR saving: {e}")

        # Try alternative save method
        level_subsystem = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
        if level_subsystem:
            try:
                level_subsystem.save_current_level()
                log("Saved via LevelEditorSubsystem")
            except:
                log("WARNING: Could not save level automatically")
                log("Please save manually in the editor (Ctrl+S)")

    log("")
    log("=" * 60)
    log("FIX COMPLETE")
    log("=" * 60)
    log("")
    log("Verify by running PIE and testing the rest menu time selection")
    log(f"Results written to: {OUTPUT_FILE}")

    return True

if __name__ == "__main__":
    fix_skymanager_in_level()
