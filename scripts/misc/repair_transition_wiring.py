# repair_transition_wiring.py
# Find disconnected VariableGet nodes in transitions and wire them properly

import unreal

def log(msg):
    print(f"[RepairWiring] {msg}")
    unreal.log_warning(f"[RepairWiring] {msg}")

ANIMBP_PATH = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

def main():
    log("=" * 70)
    log("REPAIRING TRANSITION WIRING")
    log("=" * 70)

    bp = unreal.load_asset(ANIMBP_PATH)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return False

    if not hasattr(unreal, 'SLFAutomationLibrary'):
        log("[ERROR] SLFAutomationLibrary not available")
        return False

    # Use C++ function to repair wiring
    log("Calling repair_transition_wiring...")
    repaired = unreal.SLFAutomationLibrary.repair_transition_wiring(bp)
    log(f"Repaired {repaired} connections")

    if repaired > 0:
        log("Compiling...")
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        log("Saving...")
        unreal.EditorAssetLibrary.save_asset(ANIMBP_PATH)
        log("Done")

    return repaired > 0

if __name__ == "__main__":
    main()
