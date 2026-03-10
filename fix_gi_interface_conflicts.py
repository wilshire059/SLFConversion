#!/usr/bin/env python3
"""
Fix GI_SoulslikeFramework interface conflicts by removing Blueprint function graphs.

The Blueprint has interface function implementations that conflict with C++ parent.
We need to remove these Blueprint function graphs so C++ implementations are used.
"""

import unreal

def log(msg):
    print(f"[FixGI] {msg}")
    unreal.log_warning(f"[FixGI] {msg}")

def main():
    bp_path = "/Game/SoulslikeFramework/Blueprints/Global/GI_SoulslikeFramework"

    # These are the interface functions that have conflicts
    conflicting_functions = [
        "SetSelectedClass",
        "GetCustomGameSettings",
        "GetSoulslikeGameInstance",
        "GetSelectedClass",
        "GetAllSlots",  # May also conflict with save/load
    ]

    log("=" * 70)
    log("FIXING GI_SoulslikeFramework INTERFACE CONFLICTS")
    log("=" * 70)

    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        log(f"ERROR: Could not load: {bp_path}")
        return

    log(f"Loaded: {bp.get_name()}")

    # Try to clear all Blueprint logic first (removes function graphs)
    log("Clearing ALL Blueprint logic (including function graphs)...")
    try:
        result = unreal.SLFAutomationLibrary.clear_all_blueprint_logic(bp)
        log(f"ClearAllBlueprintLogic result: {result}")
    except Exception as e:
        log(f"ERROR clearing all logic: {e}")

        # Fallback: try removing specific functions
        log("Trying to remove specific conflicting functions...")
        for func_name in conflicting_functions:
            try:
                result = unreal.SLFAutomationLibrary.remove_function(bp, func_name)
                log(f"  RemoveFunction({func_name}): {result}")
            except Exception as e2:
                log(f"  Failed to remove {func_name}: {e2}")

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
    if errors and len(errors) > 0:
        log(f"Compile errors: {errors}")
    else:
        log("No compile errors - SUCCESS!")

    log("=" * 70)
    log("DONE")
    log("=" * 70)

if __name__ == "__main__":
    main()
