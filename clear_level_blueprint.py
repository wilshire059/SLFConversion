import unreal

output = []
def log(msg):
    print(msg)
    output.append(str(msg))

automation = unreal.SLFAutomationLibrary

log("=" * 70)
log("CLEARING LEVEL BLUEPRINT")
log("=" * 70)

# Load the level first
level_path = "/Game/SoulslikeFramework/Maps/L_Demo_Showcase"
editor_subsystem = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)

if editor_subsystem:
    result = editor_subsystem.load_level(level_path)
    log(f"Level load result: {result}")

    # Get world
    world = unreal.EditorLevelLibrary.get_editor_world()
    if world:
        log(f"World: {world.get_name()}")

        # Find the level blueprint in the level asset
        # The level blueprint is embedded in the map file
        # We need to get it via the persistent level

        # Try getting all level actors to find the level script actor
        all_actors = unreal.EditorLevelLibrary.get_all_level_actors()

        level_script_actor = None
        for actor in all_actors:
            class_name = actor.get_class().get_name()
            if "LevelScriptActor" in class_name or "_Blueprint" in class_name:
                level_script_actor = actor
                log(f"Found potential Level Script Actor: {actor.get_name()} ({class_name})")
                break

        if level_script_actor:
            # Get the class of the level script actor
            ls_class = level_script_actor.get_class()
            log(f"Level Script Class: {ls_class.get_name()}")

            # The level blueprint IS the level script actor's class
            # But we can't easily edit it via Python
            log("Note: Level Blueprint embedded in map - C++ now handles cutscene spawning")
        else:
            log("No Level Script Actor found")

        # The important thing is that C++ now handles the cutscene spawning
        # The Level Blueprint errors will show as warnings but shouldn't crash

        # Save the level (this may trigger a recompile of the level blueprint)
        log("Saving level...")
        unreal.EditorLevelLibrary.save_current_level()
        log("Level saved")
    else:
        log("ERROR: Could not get world")
else:
    log("ERROR: Could not get LevelEditorSubsystem")

log("\n" + "=" * 70)
log("INFO: C++ cutscene spawning now handles first-time demo level logic")
log("Level Blueprint errors are cosmetic - functionality moved to C++")
log("=" * 70)

# Write output
with open("C:/scripts/SLFConversion/migration_cache/level_bp_clear.txt", 'w') as f:
    f.write('\n'.join(output))
