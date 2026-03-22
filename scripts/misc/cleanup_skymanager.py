"""
Cleanup B_SkyManager - Remove extra instances

After the fix script, there may be duplicate B_SkyManager instances.
This script removes all but one, keeping the one with valid components.
"""

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/skymanager_cleanup.txt"

def log(msg):
    """Log to file and UE"""
    unreal.log(msg)
    with open(OUTPUT_FILE, "a", encoding="utf-8") as f:
        f.write(msg + "\n")

def cleanup_skymanager():
    # Clear previous output
    with open(OUTPUT_FILE, "w", encoding="utf-8") as f:
        f.write("")

    log("=" * 60)
    log("Cleanup B_SkyManager - Remove Duplicates")
    log("=" * 60)

    level_path = "/Game/SoulslikeFramework/Maps/L_Demo_Showcase"

    # Load level
    log(f"Opening level: {level_path}")
    loaded_world = unreal.EditorLoadingAndSavingUtils.load_map(level_path)
    if loaded_world:
        log(f"Loaded world: {loaded_world.get_name()}")
    else:
        log("ERROR: Could not load level")
        return False

    # Get world
    editor_subsystem = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    world = editor_subsystem.get_editor_world() if editor_subsystem else None

    if not world:
        log("ERROR: Could not get world")
        return False

    log(f"Working in world: {world.get_name()}")

    # Find all B_SkyManager instances
    log("")
    log("Searching for B_SkyManager instances...")
    all_actors = unreal.GameplayStatics.get_all_actors_of_class(world, unreal.Actor)

    skymanagers = []
    for actor in all_actors:
        class_name = actor.get_class().get_name()
        if "SkyManager" in class_name:
            comps = actor.get_components_by_class(unreal.ActorComponent)
            loc = actor.get_actor_location()
            skymanagers.append((actor, len(comps), loc))
            log(f"  Found: {actor.get_name()}")
            log(f"    Components: {len(comps)}")
            log(f"    Location: ({loc.x:.1f}, {loc.y:.1f}, {loc.z:.1f})")

    log(f"")
    log(f"Total B_SkyManager instances: {len(skymanagers)}")

    if len(skymanagers) <= 1:
        log("Only one or zero instances found. No cleanup needed.")
        return True

    # Keep the first one with the most components, delete others
    # Sort by component count (descending), then by not being at origin
    def sort_key(item):
        actor, comp_count, loc = item
        at_origin = abs(loc.x) < 1 and abs(loc.y) < 1 and abs(loc.z) < 1
        return (-comp_count, at_origin)  # More components first, not-at-origin first

    skymanagers.sort(key=sort_key)

    keep_actor, keep_comps, keep_loc = skymanagers[0]
    log("")
    log(f"Keeping: {keep_actor.get_name()} ({keep_comps} components)")
    log(f"  Location: ({keep_loc.x:.1f}, {keep_loc.y:.1f}, {keep_loc.z:.1f})")

    # Delete the rest
    deleted_count = 0
    for actor, comp_count, loc in skymanagers[1:]:
        log(f"Deleting: {actor.get_name()} ({comp_count} components)")
        actor.destroy_actor()
        deleted_count += 1

    log("")
    log(f"Deleted {deleted_count} duplicate instance(s)")

    # Save level
    log("")
    log("Saving level...")
    try:
        saved = unreal.EditorLoadingAndSavingUtils.save_map(world, level_path)
        if saved:
            log("Level saved successfully!")
        else:
            log("WARNING: save_map returned False")
    except Exception as e:
        log(f"Save error: {e}")

    log("")
    log("=" * 60)
    log("CLEANUP COMPLETE")
    log("=" * 60)
    log(f"Results written to: {OUTPUT_FILE}")

    return True

if __name__ == "__main__":
    cleanup_skymanager()
