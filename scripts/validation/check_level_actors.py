import unreal

# Load the demo level
level_path = "/Game/SoulslikeFramework/Maps/L_Demo_Showcase"

print(f"Checking level: {level_path}")

# Load the level
editor_subsystem = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
if editor_subsystem:
    editor_subsystem.load_level(level_path)
    print("Level loaded")

# Get all actors in the level
world = unreal.EditorLevelLibrary.get_editor_world()
if world:
    actors = unreal.EditorLevelLibrary.get_all_level_actors()
    print(f"Total actors: {len(actors)}")

    # Find B_SequenceActor instances
    sequence_actors = []
    for actor in actors:
        actor_name = actor.get_name()
        actor_class = actor.get_class().get_name()

        if "sequence" in actor_name.lower() or "sequence" in actor_class.lower():
            sequence_actors.append((actor_name, actor_class))
            print(f"  Found: {actor_name} (Class: {actor_class})")

    if not sequence_actors:
        print("No SequenceActor found in level!")

        # Also check for any cinematic-related actors
        print("\nChecking for cinematic actors:")
        for actor in actors:
            actor_name = actor.get_name()
            actor_class = actor.get_class().get_name()
            if "cine" in actor_name.lower() or "cine" in actor_class.lower():
                print(f"  Found: {actor_name} (Class: {actor_class})")
else:
    print("ERROR: Could not get editor world")
