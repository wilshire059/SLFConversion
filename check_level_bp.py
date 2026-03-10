import unreal

output = []
def log(msg):
    print(msg)
    output.append(str(msg))

automation = unreal.SLFAutomationLibrary

log("=" * 70)
log("CHECKING LEVEL BLUEPRINT")
log("=" * 70)

# Load the level
level_path = "/Game/SoulslikeFramework/Maps/L_Demo_Showcase"
editor_subsystem = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)

if editor_subsystem:
    result = editor_subsystem.load_level(level_path)
    log(f"Level load result: {result}")

    # Get the level blueprint using EditorAssetLibrary
    level_bp_path = "/Game/SoulslikeFramework/Maps/L_Demo_Showcase"

    # Try to get world
    world = unreal.EditorLevelLibrary.get_editor_world()
    if world:
        log(f"World: {world.get_name()}")

        # Get level script actor
        actors = unreal.EditorLevelLibrary.get_all_level_actors()
        log(f"Total actors in level: {len(actors)}")

        # Find level script actor
        for actor in actors:
            if "LevelScriptActor" in actor.get_class().get_name():
                log(f"Level Script Actor: {actor.get_name()}")
                log(f"Level Script Class: {actor.get_class().get_name()}")
                break

        # Try to get the level blueprint from persistent level
        persistent_level = world.get_outermost()
        log(f"Outer: {persistent_level.get_name() if persistent_level else 'None'}")
else:
    log("ERROR: Could not get LevelEditorSubsystem")

log("\n" + "=" * 70)
log("CHECK COMPLETE")
log("=" * 70)

# Write output
with open("C:/scripts/SLFConversion/migration_cache/level_bp_check.txt", 'w') as f:
    f.write('\n'.join(output))
