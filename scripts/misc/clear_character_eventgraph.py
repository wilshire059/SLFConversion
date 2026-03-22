"""
Clear the EventGraph of B_Soulslike_Character so C++ input handling takes over.
Run in Unreal Editor: py clear_character_eventgraph.py
"""
import unreal

def log(msg):
    unreal.log_warning(msg)

def main():
    log("=" * 70)
    log("CLEARING B_SOULSLIKE_CHARACTER EVENT GRAPH")
    log("=" * 70)

    bp_path = "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character"
    bp = unreal.load_asset(bp_path)

    if not bp:
        log(f"[ERROR] Could not load {bp_path}")
        return

    log(f"Loaded: {bp.get_name()}")

    # Clear all graphs (including collapsed) but keep variables using clear_graphs_keep_variables
    log("Clearing all graphs (keeping variables)...")
    try:
        success = unreal.SLFAutomationLibrary.clear_graphs_keep_variables(bp)
        if success:
            log("[OK] All graphs cleared (variables preserved)")
        else:
            log("[WARN] clear_graphs_keep_variables returned False")
    except Exception as e:
        log(f"[ERROR] Exception: {e}")
        return

    # Compile and save
    log("Compiling Blueprint...")
    unreal.BlueprintEditorLibrary.compile_blueprint(bp)
    log("[OK] Blueprint compiled")

    log("Saving asset...")
    unreal.EditorAssetLibrary.save_asset(bp_path, only_if_is_dirty=False)
    log("[OK] Asset saved")

    log("")
    log("=" * 70)
    log("SUCCESS! EventGraph cleared.")
    log("")
    log("The C++ input handling in ASLFSoulslikeCharacter will now take over.")
    log("Restart PIE to test two-hand stance:")
    log("  - Gamepad: Hold Y + Press RT (right hand)")
    log("  - Gamepad: Hold Y + Press LT (left hand)")
    log("  - Keyboard: Hold E + Click LMB (right hand)")
    log("  - Keyboard: Hold E + Click RMB (left hand)")
    log("=" * 70)

if __name__ == "__main__":
    main()
