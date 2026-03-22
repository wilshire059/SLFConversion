#!/usr/bin/env python3
"""Reset the FirstTimeOnDemoLevel flag to true so the cutscene plays on next PIE"""

import unreal

def log(msg):
    print(f"[ResetCutscene] {msg}")
    unreal.log_warning(f"[ResetCutscene] {msg}")

def main():
    log("=" * 70)
    log("RESETTING CUTSCENE FLAG")
    log("=" * 70)

    # 1. Reset GI_SoulslikeFramework Blueprint CDO
    gi_path = "/Game/SoulslikeFramework/Blueprints/Global/GI_SoulslikeFramework"
    gi_bp = unreal.EditorAssetLibrary.load_asset(gi_path)

    if not gi_bp:
        log("[ERROR] Could not load GI_SoulslikeFramework Blueprint!")
        return

    log(f"Loaded: {gi_bp.get_name()}")

    gen_class = gi_bp.generated_class()
    if not gen_class:
        log("[ERROR] No generated class!")
        return

    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        log("[ERROR] No CDO!")
        return

    # Check current value
    try:
        current = cdo.get_editor_property("first_time_on_demo_level")
        log(f"Current first_time_on_demo_level: {current}")
    except:
        log("Property name might be different, trying alternatives...")
        current = None

    # Set to True
    try:
        cdo.set_editor_property("first_time_on_demo_level", True)
        log("Set first_time_on_demo_level = True")
    except Exception as e:
        log(f"Error setting property: {e}")
        # Try alternative name
        try:
            cdo.set_editor_property("FirstTimeOnDemoLevel", True)
            log("Set FirstTimeOnDemoLevel = True (alt name)")
        except Exception as e2:
            log(f"Error with alt name: {e2}")

    # Verify
    try:
        new_val = cdo.get_editor_property("first_time_on_demo_level")
        log(f"Verified: first_time_on_demo_level = {new_val}")
    except:
        pass

    # Save the Blueprint
    save_result = unreal.EditorAssetLibrary.save_asset(gi_path)
    log(f"Saved GI_SoulslikeFramework: {'SUCCESS' if save_result else 'FAILED'}")

    # 2. Also verify B_SequenceActor has the sequence set
    log("\n--- Verifying B_SequenceActor ---")
    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/B_SequenceActor"
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)

    if bp:
        gen_class = bp.generated_class()
        if gen_class:
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                try:
                    seq = cdo.get_editor_property("sequence_to_play")
                    if seq:
                        log(f"sequence_to_play: {seq.get_name()}")
                    else:
                        # Set the default sequence
                        log("sequence_to_play is None - setting LS_ShowcaseRoom")
                        seq_path = "/Game/SoulslikeFramework/Cinematics/LS_ShowcaseRoom"
                        seq_asset = unreal.EditorAssetLibrary.load_asset(seq_path)
                        if seq_asset:
                            cdo.set_editor_property("sequence_to_play", seq_asset)
                            log(f"Set sequence_to_play = {seq_asset.get_name()}")
                            unreal.EditorAssetLibrary.save_asset(bp_path)
                            log("Saved B_SequenceActor")
                except Exception as e:
                    log(f"Error checking sequence: {e}")

    log("\n" + "=" * 70)
    log("DONE - Cutscene should play on next PIE")
    log("=" * 70)

if __name__ == "__main__":
    main()
