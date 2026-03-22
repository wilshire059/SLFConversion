#!/usr/bin/env python3
"""
Remove interface implementation from GI_SoulslikeFramework.

The Blueprint has BPI_GameInstance interface graphs that conflict with C++ parent.
We need to remove the interface implementation from the Blueprint.
C++ parent (SLFGameInstance) already implements the interface.
"""

import unreal

def log(msg):
    print(f"[RemoveInterface] {msg}")
    unreal.log_warning(f"[RemoveInterface] {msg}")

def main():
    bp_path = "/Game/SoulslikeFramework/Blueprints/Global/GI_SoulslikeFramework"

    log("=" * 70)
    log("REMOVING INTERFACE FROM GI_SoulslikeFramework")
    log("=" * 70)

    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        log(f"ERROR: Could not load: {bp_path}")
        return

    log(f"Loaded: {bp.get_name()}")

    # Remove all implemented interfaces from the Blueprint
    log("Removing all implemented interfaces...")
    try:
        result = unreal.SLFAutomationLibrary.remove_implemented_interfaces(bp)
        log(f"RemoveImplementedInterfaces result: {result} interfaces removed")
    except Exception as e:
        log(f"ERROR removing interfaces: {e}")
        return

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
