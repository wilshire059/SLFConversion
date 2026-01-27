"""
Extract B_StatusEffectArea Blueprint CDO defaults.
"""

import unreal
import os

def log(msg):
    print(msg)
    unreal.log(msg)

def extract_cdo():
    """Extract B_StatusEffectArea CDO defaults."""

    bp_path = "/Game/SoulslikeFramework/Data/StatusEffects/B_StatusEffectArea"
    log(f"=== Extracting B_StatusEffectArea CDO ===")

    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        log(f"Failed to load: {bp_path}")
        return

    log(f"Loaded: {bp.get_class().get_name()}")

    # Get generated class
    gen_class = bp.generated_class()
    if not gen_class:
        log("No generated class")
        return

    log(f"Generated class: {gen_class.get_name()}")

    # Get CDO
    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        log("No CDO")
        return

    log(f"CDO: {cdo.get_name()}")

    # Try to get StatusEffectToApply from CDO
    try:
        status_effect = cdo.get_editor_property("StatusEffectToApply")
        log(f"StatusEffectToApply (CDO): {status_effect}")
        if status_effect:
            log(f"  Path: {status_effect.get_path_name()}")
    except Exception as e:
        log(f"Error getting StatusEffectToApply: {e}")

    # Try EffectRank
    try:
        rank = cdo.get_editor_property("EffectRank")
        log(f"EffectRank (CDO): {rank}")
    except Exception as e:
        log(f"Error getting EffectRank: {e}")

    # Export the Blueprint to text to see variables
    log("\n=== Blueprint export_text ===")
    try:
        text = bp.export_text()
        # Search for StatusEffectToApply in the text
        for line in text.split('\n'):
            if 'StatusEffect' in line or 'EffectRank' in line:
                log(line[:200])
    except Exception as e:
        log(f"Error exporting: {e}")

    # Check if the level instances might have StatusEffectToApply set per-instance
    log("\n=== Checking level instances again ===")
    level_path = "/Game/SoulslikeFramework/Maps/L_Demo_Showcase"

    level_subsystem = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    if level_subsystem:
        level_subsystem.load_level(level_path)

    actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    all_actors = actor_subsystem.get_all_level_actors()

    for actor in all_actors:
        class_name = actor.get_class().get_name()
        if "StatusEffectArea" in class_name:
            actor_name = actor.get_name()
            log(f"\n{actor_name}:")

            # Export actor to text
            try:
                actor_text = actor.export_text()
                for line in actor_text.split('\n'):
                    if 'StatusEffect' in line or 'EffectRank' in line:
                        log(f"  {line[:200]}")
            except Exception as e:
                log(f"  Error: {e}")

# Run
extract_cdo()
