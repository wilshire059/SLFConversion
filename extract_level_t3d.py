"""
Export level actors to text format and search for StatusEffectArea data.
"""

import unreal
import os
import json

OUTPUT_DIR = "C:/scripts/SLFConversion/migration_cache"

def log(msg):
    print(msg)
    unreal.log(msg)

def extract_level_data():
    """Export level via package export and search for StatusEffectArea config."""

    level_path = "/Game/SoulslikeFramework/Maps/L_Demo_Showcase"
    log(f"=== Extracting level data ===")
    log(f"Level: {level_path}")

    # Load the level using new API
    level_subsystem = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    if level_subsystem:
        result = level_subsystem.load_level(level_path)
        log(f"Level load via LevelEditorSubsystem: {result}")

    # Get all actors
    actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    all_actors = actor_subsystem.get_all_level_actors()
    log(f"Total actors: {len(all_actors)}")

    results = []

    for actor in all_actors:
        class_name = actor.get_class().get_name()

        if "StatusEffectArea" in class_name:
            actor_name = actor.get_name()
            log(f"\n=== {actor_name} ({class_name}) ===")

            instance_data = {
                "actor_name": actor_name,
                "class_name": class_name,
                "location": {
                    "x": actor.get_actor_location().x,
                    "y": actor.get_actor_location().y,
                    "z": actor.get_actor_location().z
                }
            }

            # Try different property names
            prop_names = [
                "StatusEffectToApply",
                "status_effect_to_apply",
                "Effect",
                "StatusEffect"
            ]

            for prop in prop_names:
                try:
                    value = actor.get_editor_property(prop)
                    if value:
                        log(f"  {prop} = {value}")
                        if hasattr(value, 'get_path_name'):
                            instance_data[prop] = value.get_path_name()
                            log(f"    Path: {value.get_path_name()}")
                        else:
                            instance_data[prop] = str(value)
                except:
                    pass

            # Try EffectRank
            try:
                rank = actor.get_editor_property("effect_rank")
                instance_data["effect_rank"] = rank
                log(f"  EffectRank = {rank}")
            except:
                pass

            try:
                rank = actor.get_editor_property("EffectRank")
                instance_data["EffectRank"] = rank
                log(f"  EffectRank = {rank}")
            except:
                pass

            # List all available properties
            log(f"\n  Available properties:")
            cls = actor.get_class()
            for prop in cls.properties():
                prop_name = str(prop.get_name())
                if "status" in prop_name.lower() or "effect" in prop_name.lower() or "rank" in prop_name.lower():
                    try:
                        val = actor.get_editor_property(prop_name)
                        log(f"    {prop_name} = {val}")
                        if val and hasattr(val, 'get_path_name'):
                            instance_data[prop_name] = val.get_path_name()
                        elif val is not None:
                            instance_data[prop_name] = str(val)
                    except Exception as e:
                        log(f"    {prop_name} = (error: {e})")

            results.append(instance_data)

    # Save results
    output_file = os.path.join(OUTPUT_DIR, "statuseffectarea_debug.json")
    with open(output_file, 'w') as f:
        json.dump(results, f, indent=2)
    log(f"\nSaved to: {output_file}")

# Run
extract_level_data()
