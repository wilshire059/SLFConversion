"""
Verify B_StatusEffectArea configuration persisted after save.
"""

import unreal

LOG_FILE = "C:/scripts/SLFConversion/migration_cache/statuseffect_verify.txt"
log_lines = []

def log(msg):
    print(msg)
    unreal.log(msg)
    log_lines.append(str(msg))

def verify():
    level_path = "/Game/SoulslikeFramework/Maps/L_Demo_Showcase"

    log("=" * 60)
    log("Verifying B_StatusEffectArea Configuration")
    log("=" * 60)

    # Load level
    level_subsystem = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    if level_subsystem:
        level_subsystem.load_level(level_path)
        log(f"Loaded: {level_path}")

    # Find instances
    actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    all_actors = actor_subsystem.get_all_level_actors()

    status_actors = []
    for actor in all_actors:
        if "StatusEffectArea" in actor.get_class().get_name():
            status_actors.append(actor)

    log(f"\nFound {len(status_actors)} instances\n")

    # Sort by X position
    sorted_actors = sorted(status_actors, key=lambda a: a.get_actor_location().x)

    # Check each
    for actor in sorted_actors:
        name = actor.get_name()
        x_pos = actor.get_actor_location().x

        try:
            effect = actor.get_editor_property("status_effect_to_apply")
            rank = actor.get_editor_property("effect_rank")

            effect_name = effect.get_name() if effect else "NONE"
            log(f"{name} (X={x_pos:.0f}): {effect_name} (Rank {rank})")
        except Exception as e:
            log(f"{name}: ERROR - {e}")

# Run
verify()

# Save log
with open(LOG_FILE, 'w') as f:
    f.write('\n'.join(log_lines))
log(f"\nLog: {LOG_FILE}")
