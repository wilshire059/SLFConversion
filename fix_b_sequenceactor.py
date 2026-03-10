#!/usr/bin/env python3
"""
Fix B_SequenceActor Blueprint after reparenting.

The Blueprint has variables that shadow C++ properties, causing compile errors.
We need to clear all Blueprint variables and logic since C++ has everything.
"""

import unreal

def log(msg):
    print(f"[FixBSequence] {msg}")
    unreal.log_warning(f"[FixBSequence] {msg}")

def main():
    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/B_SequenceActor"

    log("=" * 70)
    log("FIXING B_SequenceActor AFTER REPARENTING")
    log("=" * 70)

    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        log(f"ERROR: Could not load: {bp_path}")
        return

    log(f"Loaded: {bp.get_name()}")

    # Clear ALL Blueprint logic (including variables)
    log("Clearing all Blueprint logic (including variables)...")
    try:
        result = unreal.SLFAutomationLibrary.clear_all_blueprint_logic(bp)
        log(f"ClearAllBlueprintLogic result: {result}")
    except Exception as e:
        log(f"ERROR: {e}")

    # Reload and compile
    log("Reloading Blueprint...")
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        log("ERROR: Could not reload Blueprint")
        return

    log("Compiling...")
    compile_result = unreal.SLFAutomationLibrary.compile_and_save(bp)
    log(f"CompileAndSave result: {compile_result}")

    # Check for errors
    errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
    if errors:
        log(f"Compile errors: {errors}")
    else:
        log("No compile errors - SUCCESS!")

    log("=" * 70)
    log("DONE")
    log("=" * 70)

if __name__ == "__main__":
    main()
