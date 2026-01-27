# diagnose_pie_character.py
# Diagnose what character is being spawned in PIE

import unreal
import time

def log(msg):
    unreal.log_warning(f"[DIAG] {msg}")

def main():
    log("=" * 70)
    log("PIE CHARACTER DIAGNOSIS")
    log("=" * 70)

    # Check the Demo map's game mode / default pawn
    log("")
    log("Checking Demo Map settings:")
    map_path = "/Game/SoulslikeFramework/Demo/DemoRoom/MAP_DemoRoom"

    # Get world settings from the map
    world = unreal.EditorLevelLibrary.get_editor_world()
    if world:
        world_settings = world.get_world_settings()
        if world_settings:
            game_mode = world_settings.default_game_mode
            log(f"  Default GameMode: {game_mode}")

    # Check B_Soulslike_Character's parent class
    log("")
    log("Checking B_Soulslike_Character:")
    bp_path = "/Game/SoulslikeFramework/Blueprints/Character/B_Soulslike_Character"
    bp = unreal.load_asset(bp_path)
    if bp:
        parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
        log(f"  Blueprint: {bp_path}")
        log(f"  Parent Class: {parent}")
    else:
        log("  [ERROR] Could not load B_Soulslike_Character")

    # Check ASLFSoulslikeCharacter C++ class exists
    log("")
    log("Checking C++ classes:")
    try:
        cpp_class = unreal.load_class(None, "/Script/SLFConversion.SLFSoulslikeCharacter")
        if cpp_class:
            log(f"  ASLFSoulslikeCharacter: EXISTS")
        else:
            log("  ASLFSoulslikeCharacter: NOT FOUND")
    except Exception as e:
        log(f"  [ERROR] {e}")

    # Start PIE and check what character is actually spawned
    log("")
    log("Starting PIE to check spawned character...")
    success = unreal.SLFInputSimulatorLibrary.start_pie()
    if not success:
        log("[ERROR] Failed to start PIE")
        return

    log("PIE started - waiting 3 seconds...")
    time.sleep(3.0)

    # Get all actors
    log("")
    log("Checking spawned actors:")
    try:
        # Use EditorFilterLibrary to get actors
        all_actors = unreal.SLFInputSimulatorLibrary.get_pie_actors()
        if all_actors:
            for actor in all_actors:
                actor_class = actor.get_class()
                class_name = actor_class.get_name()
                if "Character" in class_name or "Pawn" in class_name or "Soulslike" in class_name:
                    log(f"  {actor.get_name()} - {class_name}")
                    # Check parent hierarchy
                    parent = actor_class.get_super_class()
                    while parent:
                        log(f"      -> {parent.get_name()}")
                        parent = parent.get_super_class()
        else:
            log("  No actors found")
    except Exception as e:
        log(f"  [ERROR] {e}")

    log("")
    log("Stopping PIE...")
    unreal.SLFInputSimulatorLibrary.stop_pie()

    log("")
    log("=" * 70)
    log("DIAGNOSIS COMPLETE")
    log("=" * 70)

if __name__ == "__main__":
    main()
