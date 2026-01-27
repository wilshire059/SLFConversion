# check_character_bp2.py
# Check the status of B_Soulslike_Character Blueprint with correct path

import unreal

def log(msg):
    unreal.log_warning(f"[CHECK] {msg}")

def main():
    log("=" * 70)
    log("B_SOULSLIKE_CHARACTER CHECK (Correct Path)")
    log("=" * 70)

    # Correct path based on file search
    bp_path = "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character"

    log(f"Loading: {bp_path}")
    try:
        asset = unreal.load_asset(bp_path)
        if asset:
            log(f"  SUCCESS: Loaded {asset}")
            parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(asset)
            log(f"  Parent Class: {parent}")
            errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(asset)
            log(f"  Compile Status:")
            for line in errors.split('\n')[:20]:
                if line.strip():
                    log(f"    {line}")
        else:
            log("  FAILED: Asset is None")
    except Exception as e:
        log(f"  ERROR: {e}")

    # Also check what character is placed in the Demo map
    log("")
    log("Checking Demo map character:")
    map_path = "/Game/SoulslikeFramework/Demo/DemoRoom/MAP_DemoRoom"
    try:
        # Load the map and check actors
        world = unreal.EditorLevelLibrary.get_editor_world()
        if world:
            # Get all actors in the level
            actors = unreal.GameplayStatics.get_all_actors_of_class(world, unreal.Character)
            for actor in actors:
                log(f"  Character in map: {actor.get_name()} ({actor.get_class().get_name()})")
    except Exception as e:
        log(f"  ERROR: {e}")

    log("")
    log("=" * 70)

if __name__ == "__main__":
    main()
