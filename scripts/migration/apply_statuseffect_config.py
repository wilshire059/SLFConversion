"""
Apply StatusEffectToApply configuration to B_StatusEffectArea instances in L_Demo_Showcase.

The 7 instances are positioned in a row, we'll assign them status effects in order.
"""

import unreal
import os

OUTPUT_LOG = "C:/scripts/SLFConversion/migration_cache/statuseffect_apply_log.txt"

log_lines = []

def log(msg):
    print(msg)
    unreal.log(msg)
    log_lines.append(msg)

# Status effect data assets in the order to apply (by increasing X position)
STATUS_EFFECTS = [
    "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Poison",
    "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Bleed",
    "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Burn",
    "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Frostbite",
    "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Corruption",
    "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Madness",
    "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Plague",
]

def apply_statuseffect_config():
    """Apply status effect assignments to level instances."""

    log("=== Applying StatusEffect Configuration ===")

    # Load the level
    level_path = "/Game/SoulslikeFramework/Maps/L_Demo_Showcase"
    log(f"Loading level: {level_path}")

    level_subsystem = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    if level_subsystem:
        level_subsystem.load_level(level_path)

    # Get all actors
    actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    all_actors = actor_subsystem.get_all_level_actors()
    log(f"Total actors: {len(all_actors)}")

    # Find all B_StatusEffectArea instances
    status_areas = []
    for actor in all_actors:
        class_name = actor.get_class().get_name()
        if "StatusEffectArea" in class_name:
            status_areas.append(actor)

    log(f"Found {len(status_areas)} B_StatusEffectArea instances")

    if len(status_areas) == 0:
        log("ERROR: No StatusEffectArea instances found!")
        return

    # Sort by X position (they're arranged in a row)
    status_areas.sort(key=lambda a: a.get_actor_location().x)

    # Load status effect assets
    log("\nLoading status effect data assets...")
    loaded_effects = []
    for path in STATUS_EFFECTS:
        asset = unreal.EditorAssetLibrary.load_asset(path)
        if asset:
            log(f"  Loaded: {asset.get_name()}")
            loaded_effects.append(asset)
        else:
            log(f"  FAILED: {path}")
            loaded_effects.append(None)

    # Apply to instances
    log("\nApplying to instances...")
    modified_count = 0

    for i, actor in enumerate(status_areas):
        actor_name = actor.get_name()
        location = actor.get_actor_location()
        log(f"\n{actor_name} (X={location.x:.0f}):")

        if i < len(loaded_effects):
            effect = loaded_effects[i]
            if effect:
                try:
                    # Set StatusEffectToApply (Python uses snake_case for C++ properties)
                    actor.set_editor_property("status_effect_to_apply", effect)
                    log(f"  Set status_effect_to_apply = {effect.get_name()}")

                    # Set EffectRank (default to 1)
                    actor.set_editor_property("effect_rank", 1)
                    log(f"  Set effect_rank = 1")

                    modified_count += 1
                except Exception as e:
                    log(f"  ERROR: {e}")
            else:
                log(f"  SKIPPED: No effect loaded for index {i}")
        else:
            log(f"  SKIPPED: No effect defined for index {i}")

    # Save the level
    log(f"\nModified {modified_count} instances")
    if modified_count > 0:
        log("Saving level...")
        try:
            # Save current level
            editor_level_lib = unreal.EditorLevelLibrary
            editor_level_lib.save_current_level()
            log("Level saved!")
        except Exception as e:
            log(f"Save error: {e}")
            # Try alternative save method
            try:
                unreal.EditorAssetLibrary.save_directory("/Game/SoulslikeFramework/Maps/")
                log("Saved via EditorAssetLibrary")
            except:
                log("Could not auto-save. Please save manually.")

    log("\n=== Done ===")

    # Write log to file
    with open(OUTPUT_LOG, 'w') as f:
        f.write('\n'.join(log_lines))

# Run
apply_statuseffect_config()
