#!/usr/bin/env python3
"""Compare overlay animation sequences between projects"""

import unreal
import json

def log(msg):
    unreal.log_warning(str(msg))

# AnimBP paths
ANIMBP_PATH = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

# Character path for spawning
CHARACTER_PATH = "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character"

def get_anim_instance_info():
    """Get animation instance info from spawned character"""
    log("\n=== ANIMATION INSTANCE CHECK ===")

    bp = unreal.EditorAssetLibrary.load_asset(CHARACTER_PATH)
    if not bp:
        log("ERROR: Could not load character BP")
        return None

    gen_class = bp.generated_class()
    if not gen_class:
        log("No generated class")
        return None

    result = {
        "animbp_path": None,
        "overlay_state_left": None,
        "overlay_state_right": None,
        "variables": {}
    }

    # Spawn instance
    try:
        world = unreal.EditorLevelLibrary.get_editor_world()
        if world:
            actor = unreal.EditorLevelLibrary.spawn_actor_from_class(gen_class, unreal.Vector(0, 0, 30000))
            if actor:
                # Get skeletal mesh component
                mesh_comps = actor.get_components_by_class(unreal.SkeletalMeshComponent)
                for mesh_comp in mesh_comps:
                    if mesh_comp.get_name() == "CharacterMesh0":
                        # Get anim instance
                        anim_instance = mesh_comp.get_anim_instance()
                        if anim_instance:
                            log(f"AnimInstance class: {anim_instance.get_class().get_name()}")
                            result["animbp_path"] = anim_instance.get_class().get_path_name()

                            # Try to get overlay states
                            try:
                                left_state = anim_instance.get_editor_property("left_hand_overlay_state")
                                result["overlay_state_left"] = str(left_state) if left_state else "None"
                                log(f"LeftHandOverlayState: {result['overlay_state_left']}")
                            except Exception as e:
                                log(f"Could not get left overlay state: {e}")

                            try:
                                right_state = anim_instance.get_editor_property("right_hand_overlay_state")
                                result["overlay_state_right"] = str(right_state) if right_state else "None"
                                log(f"RightHandOverlayState: {result['overlay_state_right']}")
                            except Exception as e:
                                log(f"Could not get right overlay state: {e}")

                            # Get anim data asset
                            try:
                                anim_data = anim_instance.get_editor_property("anim_data_asset")
                                if anim_data:
                                    result["anim_data_asset"] = anim_data.get_path_name()
                                    log(f"AnimDataAsset: {anim_data.get_name()}")
                            except Exception as e:
                                log(f"Could not get anim data asset: {e}")

                            # List available properties
                            log("\n--- Available AnimInstance properties: ---")
                            anim_class = anim_instance.get_class()
                            # Get property names
                            try:
                                props = dir(anim_instance)
                                overlay_props = [p for p in props if 'overlay' in p.lower() or 'hand' in p.lower() or 'anim' in p.lower()]
                                for prop in overlay_props[:20]:
                                    try:
                                        val = getattr(anim_instance, prop)
                                        if not callable(val):
                                            log(f"  {prop}: {str(val)[:80]}")
                                    except:
                                        pass
                            except Exception as e:
                                log(f"Error listing properties: {e}")

                actor.destroy_actor()
    except Exception as e:
        log(f"Spawn error: {e}")

    return result

def check_animbp_directly():
    """Check AnimBP asset directly"""
    log("\n=== ANIMBP ASSET CHECK ===")

    animbp = unreal.EditorAssetLibrary.load_asset(ANIMBP_PATH)
    if not animbp:
        log("ERROR: Could not load AnimBP")
        return None

    log(f"AnimBP class: {animbp.get_class().get_name()}")

    result = {
        "path": ANIMBP_PATH,
        "class": animbp.get_class().get_name(),
        "skeleton": None,
        "parent_class": None
    }

    # Get skeleton
    try:
        skeleton = animbp.get_editor_property("target_skeleton")
        if skeleton:
            result["skeleton"] = skeleton.get_name()
            log(f"Target skeleton: {skeleton.get_name()}")
    except Exception as e:
        log(f"Could not get skeleton: {e}")

    # Get parent class
    try:
        parent = animbp.get_editor_property("parent_class")
        if parent:
            result["parent_class"] = parent.get_name()
            log(f"Parent class: {parent.get_name()}")
    except Exception as e:
        log(f"Could not get parent: {e}")

    return result

def check_overlay_animations_folder():
    """Check overlay animation sequences in content folder"""
    log("\n=== OVERLAY ANIMATION SEQUENCES ===")

    # Search for overlay-related animation sequences
    overlay_paths = [
        "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/Arms/",
        "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/OneHanded/",
        "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/TwoHanded/",
        "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/Shield/",
    ]

    results = {}

    for folder in overlay_paths:
        try:
            assets = unreal.EditorAssetLibrary.list_assets(folder, recursive=True)
            folder_name = folder.split("/")[-2]
            results[folder_name] = []

            log(f"\n--- {folder_name} ---")
            for asset_path in assets[:20]:  # Limit to first 20
                asset_name = asset_path.split("/")[-1].split(".")[0]
                results[folder_name].append(asset_name)
                log(f"  {asset_name}")
        except Exception as e:
            log(f"  Error listing {folder}: {e}")

    return results

def main():
    log("=" * 70)
    log("OVERLAY ANIMATION COMPARISON")
    log("=" * 70)

    results = {
        "project": "SLFConversion",
        "anim_instance": None,
        "animbp": None,
        "animation_folders": None
    }

    # Check AnimBP
    results["animbp"] = check_animbp_directly()

    # Check runtime anim instance
    results["anim_instance"] = get_anim_instance_info()

    # Check overlay animation folders
    results["animation_folders"] = check_overlay_animations_folder()

    # Save results
    output_path = "C:/scripts/slfconversion/migration_cache/overlay_anims.json"
    with open(output_path, 'w') as f:
        json.dump(results, f, indent=2)

    log(f"\n\nSaved to: {output_path}")
    log("\n" + "=" * 70)

if __name__ == "__main__":
    main()
