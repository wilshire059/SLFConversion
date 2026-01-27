"""
Extract B_StatusEffectArea instance data from bp_only level.

This script runs in the bp_only project to extract:
- Actor locations
- StatusEffectToApply assignments
- EffectRank values

Output is saved to migration_cache for later application.
"""

import unreal
import json
import os

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/statuseffectarea_instances.json"

def log(msg):
    print(msg)
    unreal.log(msg)

def extract_statuseffectarea_instances():
    """Extract B_StatusEffectArea instance data from L_Demo_Showcase."""

    log("=== Extracting B_StatusEffectArea Instances ===")

    # Load the level - correct path is Maps not Demo/_Levels
    level_path = "/Game/SoulslikeFramework/Maps/L_Demo_Showcase"
    log(f"Loading level: {level_path}")

    editor_level_lib = unreal.EditorLevelLibrary
    success = editor_level_lib.load_level(level_path)
    log(f"Level load result: {success}")

    # Get all actors in the level
    actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    all_actors = actor_subsystem.get_all_level_actors()
    log(f"Total actors in level: {len(all_actors)}")

    instances = []

    for actor in all_actors:
        class_name = actor.get_class().get_name()

        # Check if it's a B_StatusEffectArea or child
        if "StatusEffectArea" in class_name:
            log(f"\nFound: {actor.get_name()} ({class_name})")

            instance_data = {
                "actor_name": actor.get_name(),
                "class_name": class_name,
                "location": {
                    "x": actor.get_actor_location().x,
                    "y": actor.get_actor_location().y,
                    "z": actor.get_actor_location().z
                },
                "rotation": {
                    "pitch": actor.get_actor_rotation().pitch,
                    "yaw": actor.get_actor_rotation().yaw,
                    "roll": actor.get_actor_rotation().roll
                },
                "status_effect_to_apply": None,
                "effect_rank": 0
            }

            # Try to get StatusEffectToApply
            try:
                status_effect = actor.get_editor_property("status_effect_to_apply")
                if status_effect:
                    # Get the asset path
                    asset_path = status_effect.get_path_name()
                    instance_data["status_effect_to_apply"] = asset_path
                    log(f"  StatusEffectToApply: {asset_path}")
                else:
                    log(f"  StatusEffectToApply: None")
            except Exception as e:
                log(f"  Error getting status_effect_to_apply: {e}")

            # Try to get EffectRank
            try:
                effect_rank = actor.get_editor_property("effect_rank")
                instance_data["effect_rank"] = effect_rank
                log(f"  EffectRank: {effect_rank}")
            except Exception as e:
                log(f"  Error getting effect_rank: {e}")

            instances.append(instance_data)

    log(f"\n=== Found {len(instances)} B_StatusEffectArea instances ===")

    # Save to file
    os.makedirs(os.path.dirname(OUTPUT_FILE), exist_ok=True)
    with open(OUTPUT_FILE, 'w') as f:
        json.dump(instances, f, indent=2)

    log(f"Saved to: {OUTPUT_FILE}")

    return instances

# Run extraction
if __name__ == "__main__":
    extract_statuseffectarea_instances()
