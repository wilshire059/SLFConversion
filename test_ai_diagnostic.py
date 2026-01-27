# test_ai_diagnostic.py
# Run a PIE test to diagnose AI blackboard issues

import unreal

def log(msg):
    print(msg)
    unreal.log(msg)

def main():
    log("=" * 70)
    log("AI DIAGNOSTIC TEST")
    log("=" * 70)

    # Load the demo level
    level_path = "/Game/SoulslikeFramework/Maps/L_Demo_Showcase"
    log(f"Loading level: {level_path}")

    # Use the new API
    subsystem = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    success = subsystem.load_level(level_path)
    log(f"Level load result: {success}")

    # Find all enemies in the level
    actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    all_actors = actor_subsystem.get_all_level_actors()
    log(f"Total actors in level: {len(all_actors)}")

    enemies = []
    for actor in all_actors:
        actor_name = actor.get_name()
        actor_class = actor.get_class().get_name()
        if "Enemy" in actor_name or "Enemy" in actor_class or "Soulslike_Enemy" in actor_name or "Soulslike_Enemy" in actor_class or "Boss" in actor_name:
            enemies.append(actor)
            log(f"Found enemy: {actor_name} (class: {actor_class})")

    log(f"Total enemies found: {len(enemies)}")

    # Check each enemy for AI components
    for enemy in enemies:
        log(f"--- Checking {enemy.get_name()} ---")

        # Check for AIBehaviorManagerComponent
        components = enemy.get_components_by_class(unreal.ActorComponent)
        log(f"  Components ({len(components)}):")

        behavior_manager = None
        for comp in components:
            comp_name = comp.get_name()
            comp_class = comp.get_class().get_name()
            if "BehaviorManager" in comp_class or "BehaviorManager" in comp_name or "AC_AI" in comp_name:
                behavior_manager = comp
                log(f"    >>> {comp_name} ({comp_class}) - BehaviorManager!")

                # Try to get the behavior tree
                try:
                    bt = comp.get_editor_property("behavior")
                    if bt:
                        log(f"        Behavior Tree: {bt.get_name()}")
                    else:
                        log(f"        WARNING: No Behavior Tree assigned!")
                except Exception as e:
                    log(f"        Error getting behavior property: {e}")

        # Check AIControllerClass
        try:
            aic_class = enemy.get_editor_property("ai_controller_class")
            if aic_class:
                log(f"  AIControllerClass: {aic_class.get_name()}")
            else:
                log(f"  AIControllerClass: None (using default)")
        except Exception as e:
            log(f"  AIControllerClass check error: {e}")

        # Check AutoPossessAI
        try:
            auto_possess = enemy.get_editor_property("auto_possess_ai")
            log(f"  AutoPossessAI: {auto_possess}")
        except Exception as e:
            log(f"  AutoPossessAI check error: {e}")

    log("=" * 70)
    log("RUNTIME DIAGNOSIS - Run PIE manually and look for:")
    log("  [AIBehaviorManager] GetBlackboard:")
    log("  [AIBehaviorManager] FAILED")
    log("=" * 70)

if __name__ == "__main__":
    main()
