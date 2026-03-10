import unreal

output = []
def log(msg):
    print(msg)
    output.append(str(msg))

automation = unreal.SLFAutomationLibrary

log("=" * 70)
log("FIXING LEVEL BLUEPRINT")
log("=" * 70)

# First rebuild C++ to pick up changes
log("Rebuilding C++ first...")

# Load the level
level_path = "/Game/SoulslikeFramework/Maps/L_Demo_Showcase"
editor_subsystem = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)

if editor_subsystem:
    result = editor_subsystem.load_level(level_path)
    log(f"Level load result: {result}")

    # Get world and level blueprint
    world = unreal.EditorLevelLibrary.get_editor_world()
    if world:
        log(f"World: {world.get_name()}")

        # Get the level script actor (Level Blueprint instance)
        level_script = world.get_level_script_actor()
        if level_script:
            log(f"Level Script Actor: {level_script.get_name()}")
            log(f"Level Script Class: {level_script.get_class().get_name()}")

            # Get the level blueprint from the level
            level = world.get_current_level()
            if level:
                level_bp = level.get_level_script_blueprint()
                if level_bp:
                    log(f"\nLevel Blueprint: {level_bp.get_name()}")
                    log(f"Level BP Class: {level_bp.get_class().get_name()}")

                    # Get compile errors
                    errors = automation.get_blueprint_compile_errors(level_bp)
                    log(f"\nCurrent compile status:\n{errors}")

                    # Try to clear the event graphs
                    log("\nClearing Level Blueprint event graphs...")
                    result = automation.clear_event_graphs(level_bp)
                    log(f"clear_event_graphs result: {result}")

                    # Compile and save
                    log("Compiling and saving...")
                    result = automation.compile_and_save(level_bp)
                    log(f"compile_and_save result: {result}")

                    # Check compile status again
                    errors = automation.get_blueprint_compile_errors(level_bp)
                    log(f"\nAfter clearing compile status:\n{errors}")

                    # Save the level
                    log("\nSaving level...")
                    unreal.EditorLevelLibrary.save_current_level()
                    log("Level saved")
                else:
                    log("ERROR: Could not get Level Blueprint")
            else:
                log("ERROR: Could not get current level")
        else:
            log("ERROR: Could not get Level Script Actor")
    else:
        log("ERROR: Could not get world")
else:
    log("ERROR: Could not get LevelEditorSubsystem")

# Write output
with open("C:/scripts/SLFConversion/migration_cache/level_bp_fix.txt", 'w') as f:
    f.write('\n'.join(output))

log("\nDone!")
