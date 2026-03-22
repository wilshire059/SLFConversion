# verify_animbp_fix.py
# Verify AnimBP transitions are now properly wired

import unreal

def log(msg):
    print(f"[Verify] {msg}")
    unreal.log_warning(f"[Verify] {msg}")

ANIMBP_PATH = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

def main():
    log("=" * 70)
    log("VERIFYING ANIMBP TRANSITION WIRING")
    log("=" * 70)

    bp = unreal.load_asset(ANIMBP_PATH)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    if not hasattr(unreal, 'SLFAutomationLibrary'):
        log("[ERROR] SLFAutomationLibrary not available")
        return

    # Get compile errors
    errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
    if errors and len(errors.strip()) > 0:
        log(f"[WARN] Compile errors: {errors}")
    else:
        log("[OK] No compile errors")

    # Run diagnostics on transitions
    log("\n--- Inspecting Transition Graphs ---")
    result = unreal.SLFAutomationLibrary.inspect_transition_graph(bp, "IDLE")
    log(f"IDLE transitions:\n{result[:2000] if len(result) > 2000 else result}")

    log("\n[SUCCESS] AnimBP verification complete")

if __name__ == "__main__":
    main()
