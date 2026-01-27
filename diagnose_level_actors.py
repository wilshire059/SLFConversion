"""
Diagnose level actors - find all actors and look for status effect related ones.
"""

import unreal
import os

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/level_actors_debug.txt"

def log(msg):
    print(msg)
    unreal.log(msg)

def diagnose_level():
    """Debug level actors."""

    log("=== Diagnosing Level Actors ===")

    # Try to load the level
    level_path = "/Game/SoulslikeFramework/Demo/_Levels/L_Demo_Showcase"
    log(f"Attempting to load level: {level_path}")

    # Use LevelEditorSubsystem instead
    level_subsystem = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    if level_subsystem:
        result = level_subsystem.load_level(level_path)
        log(f"Level load result: {result}")
    else:
        log("Could not get LevelEditorSubsystem")

    # Get all actors
    actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    all_actors = actor_subsystem.get_all_level_actors()
    log(f"Total actors found: {len(all_actors)}")

    results = []
    results.append(f"Total actors: {len(all_actors)}\n")
    results.append("=" * 50 + "\n")

    # List all actors with classes
    status_actors = []
    for actor in all_actors:
        class_name = actor.get_class().get_name()
        actor_name = actor.get_name()

        # Look for anything status effect related
        if "status" in class_name.lower() or "status" in actor_name.lower() or "effect" in class_name.lower():
            status_actors.append(f"{actor_name} - {class_name}")

        # Also look for B_ prefix actors that might be our Blueprints
        if class_name.startswith("B_") and "Area" in class_name:
            status_actors.append(f"{actor_name} - {class_name}")

    results.append("Status Effect Related Actors:\n")
    for sa in status_actors:
        results.append(f"  {sa}\n")
        log(sa)

    if not status_actors:
        results.append("  NONE FOUND\n")
        log("No status effect actors found")

    results.append("\n" + "=" * 50 + "\n")
    results.append("All actor classes (unique):\n")

    # Get unique class names
    unique_classes = set()
    for actor in all_actors:
        unique_classes.add(actor.get_class().get_name())

    for cn in sorted(unique_classes):
        results.append(f"  {cn}\n")

    # Write to file
    os.makedirs(os.path.dirname(OUTPUT_FILE), exist_ok=True)
    with open(OUTPUT_FILE, 'w') as f:
        f.writelines(results)

    log(f"Saved debug output to: {OUTPUT_FILE}")

# Run
diagnose_level()
