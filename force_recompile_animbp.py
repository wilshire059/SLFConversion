#!/usr/bin/env python3
"""
Force full recompilation of ABP_SoulslikeBossNew by:
1. Marking as modified
2. Clearing compiled data
3. Forcing full recompile
"""

import unreal

def log(msg):
    print(f"[ForceRecompile] {msg}")
    unreal.log_warning(f"[ForceRecompile] {msg}")

def main():
    log("=" * 70)
    log("FORCE RECOMPILING ABP_SoulslikeBossNew")
    log("=" * 70)

    abp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeBossNew"

    log(f"Loading: {abp_path}")
    abp = unreal.EditorAssetLibrary.load_asset(abp_path)
    if not abp:
        log(f"ERROR: Could not load: {abp_path}")
        return

    log(f"Loaded: {abp.get_name()}")

    # Get the Blueprint class
    bp = unreal.AnimBlueprint.cast(abp)
    if not bp:
        log("ERROR: Could not cast to AnimBlueprint")
        return

    log(f"AnimBlueprint: {bp.get_name()}")

    # Check current state
    log("")
    log("Current state:")
    try:
        has_logic = unreal.SLFAutomationLibrary.has_event_graph_logic(bp)
        log(f"  HasEventGraphLogic: {has_logic}")
    except Exception as e:
        log(f"  HasEventGraphLogic error: {e}")

    # Mark as modified to force recompilation
    log("")
    log("Step 1: Mark Blueprint as modified...")
    try:
        bp.modify(True)
        log("  Marked as modified")
    except Exception as e:
        log(f"  Mark modified error: {e}")

    # Try to access and clear the skeleton generated class
    log("")
    log("Step 2: Getting generated class info...")
    try:
        gen_class = bp.generated_class()
        log(f"  Generated class: {gen_class.get_name() if gen_class else 'None'}")

        skel_class = bp.get_editor_property('skeleton_generated_class')
        log(f"  Skeleton class: {skel_class.get_name() if skel_class else 'None'}")
    except Exception as e:
        log(f"  Class info error: {e}")

    # Clear event graphs one more time
    log("")
    log("Step 3: Clear event graphs again...")
    try:
        cleared = unreal.SLFAutomationLibrary.clear_event_graphs(bp)
        log(f"  Cleared {cleared} nodes")
    except Exception as e:
        log(f"  Clear error: {e}")

    # Request full recompilation
    log("")
    log("Step 4: Requesting full recompilation...")
    try:
        # Use BlueprintEditorLibrary for compilation
        compile_result = unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        log(f"  BlueprintEditorLibrary.compile_blueprint result: {compile_result}")
    except Exception as e:
        log(f"  Compile error: {e}")

    # Also try SLFAutomationLibrary compile
    log("")
    log("Step 5: SLFAutomationLibrary compile and save...")
    try:
        result = unreal.SLFAutomationLibrary.compile_and_save(bp)
        log(f"  CompileAndSave result: {result}")
    except Exception as e:
        log(f"  CompileAndSave error: {e}")

    # Save the asset
    log("")
    log("Step 6: Saving asset...")
    try:
        save_result = unreal.EditorAssetLibrary.save_asset(abp_path)
        log(f"  Save result: {save_result}")
    except Exception as e:
        log(f"  Save error: {e}")

    # Check final state
    log("")
    log("Final state:")
    try:
        # Reload
        abp = unreal.EditorAssetLibrary.load_asset(abp_path)
        has_logic = unreal.SLFAutomationLibrary.has_event_graph_logic(abp)
        log(f"  HasEventGraphLogic: {has_logic}")

        errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(abp)
        log(f"  Compile errors: {errors if errors else 'None'}")
    except Exception as e:
        log(f"  Final state error: {e}")

    log("")
    log("=" * 70)
    log("DONE - Restart editor completely and test PIE")
    log("=" * 70)

if __name__ == "__main__":
    main()
