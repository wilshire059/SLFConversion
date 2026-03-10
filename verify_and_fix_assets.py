#!/usr/bin/env python3
"""
Verify B_SequenceActor parent and force re-clear ABP_SoulslikeBossNew with explicit save.
"""

import unreal

def log(msg):
    print(f"[VerifyFix] {msg}")
    unreal.log_warning(f"[VerifyFix] {msg}")

def main():
    log("=" * 70)
    log("VERIFYING AND FIXING ASSETS")
    log("=" * 70)

    # 1. Check B_SequenceActor parent
    log("")
    log("1. CHECKING B_SequenceActor...")
    seq_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/B_SequenceActor"
    seq_bp = unreal.EditorAssetLibrary.load_asset(seq_path)
    if seq_bp:
        try:
            parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(seq_bp)
            log(f"   Parent Class: {parent}")

            # Check if it has EventGraph logic
            has_logic = unreal.SLFAutomationLibrary.has_event_graph_logic(seq_bp)
            log(f"   HasEventGraphLogic: {has_logic}")

            if has_logic:
                log("   WARNING: B_SequenceActor still has EventGraph logic!")
                log("   Clearing EventGraph...")
                cleared = unreal.SLFAutomationLibrary.clear_event_graphs(seq_bp)
                log(f"   Cleared {cleared} nodes")

                # Save
                unreal.EditorAssetLibrary.save_asset(seq_path)
                log("   Saved B_SequenceActor")
        except Exception as e:
            log(f"   Error: {e}")
    else:
        log(f"   ERROR: Could not load {seq_path}")

    # 2. Force clear ABP_SoulslikeBossNew with explicit save
    log("")
    log("2. FORCE CLEARING ABP_SoulslikeBossNew...")
    abp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeBossNew"
    abp = unreal.EditorAssetLibrary.load_asset(abp_path)
    if abp:
        try:
            # Check current state
            has_logic = unreal.SLFAutomationLibrary.has_event_graph_logic(abp)
            log(f"   HasEventGraphLogic BEFORE: {has_logic}")

            # Use the most aggressive clear
            log("   Calling ClearAllEventGraphNodes...")
            result = unreal.SLFAutomationLibrary.clear_all_event_graph_nodes(abp_path, True)
            log(f"   ClearAllEventGraphNodes result: {result}")

            # Reload
            abp = unreal.EditorAssetLibrary.load_asset(abp_path)

            # Check after
            has_logic = unreal.SLFAutomationLibrary.has_event_graph_logic(abp)
            log(f"   HasEventGraphLogic AFTER: {has_logic}")

            # Explicit save using multiple methods
            log("   Saving via EditorAssetLibrary...")
            save_result = unreal.EditorAssetLibrary.save_asset(abp_path)
            log(f"   EditorAssetLibrary.save_asset result: {save_result}")

            # Also try save_loaded_asset
            log("   Saving via save_loaded_asset...")
            save_result2 = unreal.EditorAssetLibrary.save_loaded_asset(abp)
            log(f"   save_loaded_asset result: {save_result2}")

        except Exception as e:
            log(f"   Error: {e}")
            import traceback
            traceback.print_exc()
    else:
        log(f"   ERROR: Could not load {abp_path}")

    # 3. Check for any other AnimBPs that might have the issue
    log("")
    log("3. CHECKING OTHER BOSS-RELATED ANIMBPS...")
    other_paths = [
        "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeBoss",
    ]
    for path in other_paths:
        bp = unreal.EditorAssetLibrary.load_asset(path)
        if bp:
            try:
                has_logic = unreal.SLFAutomationLibrary.has_event_graph_logic(bp)
                log(f"   {path.split('/')[-1]}: HasEventGraphLogic = {has_logic}")
                if has_logic:
                    log(f"      Clearing...")
                    unreal.SLFAutomationLibrary.clear_all_event_graph_nodes(path, True)
                    unreal.EditorAssetLibrary.save_asset(path)
                    log(f"      Cleared and saved")
            except Exception as e:
                log(f"   {path.split('/')[-1]}: Error - {e}")
        else:
            log(f"   {path.split('/')[-1]}: Not found (OK)")

    log("")
    log("=" * 70)
    log("DONE - Restart editor and test PIE")
    log("=" * 70)

if __name__ == "__main__":
    main()
