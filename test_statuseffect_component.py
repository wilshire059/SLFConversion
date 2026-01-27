"""
Test that StatusEffectManager component exists on player character.
"""

import unreal

LOG_FILE = "C:/scripts/SLFConversion/migration_cache/statuseffect_component_test.txt"
log_lines = []

def log(msg):
    print(msg)
    unreal.log(msg)
    log_lines.append(str(msg))

def test():
    log("=" * 60)
    log("Testing StatusEffectManager Component")
    log("=" * 60)

    # Get player pawn
    world = unreal.EditorLevelLibrary.get_editor_world()
    if not world:
        log("ERROR: No editor world")
        return

    # Get all characters
    actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    all_actors = actor_subsystem.get_all_level_actors()

    for actor in all_actors:
        class_name = actor.get_class().get_name()

        # Check characters
        if "Soulslike_Character" in class_name or "BaseCharacter" in class_name:
            log(f"\n{actor.get_name()} ({class_name})")

            # List all components
            components = actor.get_components_by_class(unreal.ActorComponent)
            log(f"  Total components: {len(components)}")

            # Look for StatusEffectManager
            found_sem = False
            for comp in components:
                comp_name = comp.get_name()
                comp_class = comp.get_class().get_name()
                if "StatusEffect" in comp_name or "StatusEffect" in comp_class:
                    log(f"  FOUND: {comp_name} ({comp_class})")
                    found_sem = True

            if not found_sem:
                log("  WARNING: No StatusEffectManager component found!")

    # Also check the CDO of the C++ class
    log("\n" + "=" * 60)
    log("Checking C++ Class Default Objects")
    log("=" * 60)

    # Try to load the C++ class
    cpp_classes = [
        "/Script/SLFConversion.SLFBaseCharacter",
        "/Script/SLFConversion.SLFSoulslikeCharacter",
    ]

    for cpp_path in cpp_classes:
        cpp_class = unreal.load_class(None, cpp_path)
        if cpp_class:
            log(f"\n{cpp_class.get_name()}:")
            cdo = unreal.get_default_object(cpp_class)
            if cdo:
                # Check for cached component
                try:
                    sem = cdo.get_editor_property("cached_status_effect_manager")
                    if sem:
                        log(f"  CachedStatusEffectManager: {sem.get_name()} ({sem.get_class().get_name()})")
                    else:
                        log("  CachedStatusEffectManager: None (will be created at runtime)")
                except Exception as e:
                    log(f"  CachedStatusEffectManager: Property access failed - {e}")
        else:
            log(f"\nFailed to load: {cpp_path}")

# Run
test()

# Save log
with open(LOG_FILE, 'w') as f:
    f.write('\n'.join(log_lines))
log(f"\nLog: {LOG_FILE}")
