"""
Debug: List all accessible properties on B_StatusEffectArea instances.
"""

import unreal

LOG_FILE = "C:/scripts/SLFConversion/migration_cache/statuseffect_props_debug.txt"
log_lines = []

def log(msg):
    print(msg)
    unreal.log(msg)
    log_lines.append(str(msg))

def debug_props():
    """List all properties on B_StatusEffectArea instances."""

    log("=== Debugging StatusEffectArea Properties ===")

    # Load the level
    level_path = "/Game/SoulslikeFramework/Maps/L_Demo_Showcase"
    level_subsystem = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    if level_subsystem:
        level_subsystem.load_level(level_path)

    # Get all actors
    actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    all_actors = actor_subsystem.get_all_level_actors()

    for actor in all_actors:
        class_name = actor.get_class().get_name()
        if "StatusEffectArea" in class_name:
            log(f"\n=== {actor.get_name()} ===")
            log(f"Class: {class_name}")
            log(f"Class path: {actor.get_class().get_path_name()}")

            # Test various property name formats
            test_props = [
                "status_effect_to_apply",
                "StatusEffectToApply",
                "statuseffecttoapply",
                "effect_rank",
                "EffectRank",
                "effectrank",
            ]

            log("\nTesting property access:")
            for prop in test_props:
                try:
                    val = actor.get_editor_property(prop)
                    log(f"  {prop}: {val}")
                except Exception as e:
                    log(f"  {prop}: FAILED - {e}")

            # Try to list properties using dir()
            log("\ndir() on actor (filtered):")
            for attr in sorted(dir(actor)):
                if "effect" in attr.lower() or "status" in attr.lower() or "rank" in attr.lower():
                    log(f"  {attr}")

            # Only check first actor
            break

    # Save log
    with open(LOG_FILE, 'w') as f:
        f.write('\n'.join(log_lines))
    log(f"\nSaved to: {LOG_FILE}")

# Run
debug_props()
