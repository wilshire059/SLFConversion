# verify_animbp_transitions.py
# Final verification of AnimBP transition fixes
import unreal

def log(msg):
    print(msg)
    unreal.log_warning(f"[VERIFY] {msg}")

def main():
    log("=" * 70)
    log("VERIFYING ANIMBP TRANSITIONS AFTER FIX")
    log("=" * 70)

    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

    bp = unreal.load_asset(animbp_path)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    if not hasattr(unreal, 'SLFAutomationLibrary'):
        log("[ERROR] SLFAutomationLibrary not available")
        return

    # Run full diagnosis
    log("")
    log("=== FULL ANIMGRAPH DIAGNOSIS ===")
    result = unreal.SLFAutomationLibrary.diagnose_anim_bp(bp)
    for line in result.split('\n'):
        log(line)

    # Specifically check IDLE to CYCLE transitions
    log("")
    log("=== CHECKING IDLE TO CYCLE TRANSITIONS ===")
    result = unreal.SLFAutomationLibrary.inspect_transition_graph(bp, "IDLE")
    for line in result.split('\n'):
        if "CYCLE" in line or "Speed" in line or "bIsAccelerating" in line or "Pin" in line or "Transition" in line:
            log(line)

    # Check CYCLE to IDLE transitions
    log("")
    log("=== CHECKING CYCLE TO IDLE TRANSITIONS ===")
    result = unreal.SLFAutomationLibrary.inspect_transition_graph(bp, "CYCLE")
    for line in result.split('\n'):
        if "IDLE" in line or "Speed" in line or "bIsAccelerating" in line or "Pin" in line or "Transition" in line:
            log(line)

    # Check variable nodes in AnimBP
    log("")
    log("=== CHECKING ANIMGRAPH VARIABLES ===")
    result = unreal.SLFAutomationLibrary.diagnose_anim_graph_nodes(bp)
    for line in result.split('\n'):
        if "Speed" in line or "bIsAccelerating" in line or "Variable" in line:
            log(line)

    log("")
    log("=== VERIFICATION COMPLETE ===")
    log("")
    log("EXPECTED STATE FOR WORKING TRANSITIONS:")
    log("  1. IDLE to CYCLE: Speed > 10 should transition to walking")
    log("  2. CYCLE to IDLE: NOT bIsAccelerating should transition back to idle")
    log("  3. All transition pins should be connected (not showing 'false')")

if __name__ == "__main__":
    main()
