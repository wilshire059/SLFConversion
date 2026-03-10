#!/usr/bin/env python3
"""
Clear ALL graphs (including function graphs) from GI_SoulslikeFramework.

The Blueprint has a function that calls GetAllSlots on SG_SaveSlots with
a mismatched return type. Need to clear all function graphs.
"""

import unreal

def log(msg):
    print(f"[ClearGI] {msg}")
    unreal.log_warning(f"[ClearGI] {msg}")

def main():
    bp_path = "/Game/SoulslikeFramework/Blueprints/Global/GI_SoulslikeFramework"

    log("=" * 70)
    log("CLEARING ALL GRAPHS FROM GI_SoulslikeFramework")
    log("=" * 70)

    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        log(f"ERROR: Could not load: {bp_path}")
        return

    log(f"Loaded: {bp.get_name()}")

    # Use ClearGraphsKeepVariables which clears all graphs but preserves variables
    # (Level BP reads variables like "First Time on Demo Level?")
    try:
        result = unreal.SLFAutomationLibrary.clear_graphs_keep_variables(bp)
        log(f"ClearGraphsKeepVariables result: {result}")

        # Reload and compile
        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if bp:
            compile_result = unreal.SLFAutomationLibrary.compile_and_save(bp)
            log(f"CompileAndSave result: {compile_result}")

    except Exception as e:
        log(f"ERROR: {e}")

    log("=" * 70)
    log("DONE")
    log("=" * 70)

if __name__ == "__main__":
    main()
