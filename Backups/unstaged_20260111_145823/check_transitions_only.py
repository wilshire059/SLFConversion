# check_transitions_only.py
# Check only the IDLE to CYCLE transitions
import unreal

def log(msg):
    print(msg)
    unreal.log_warning(f"[TRANS] {msg}")

def main():
    log("=" * 70)
    log("CHECKING IDLE TO CYCLE TRANSITIONS")
    log("=" * 70)

    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

    bp = unreal.load_asset(animbp_path)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    if not hasattr(unreal, 'SLFAutomationLibrary'):
        log("[ERROR] SLFAutomationLibrary not available")
        return

    # Check all transitions
    log("")
    log("=== ALL TRANSITIONS ===")
    result = unreal.SLFAutomationLibrary.inspect_transition_graph(bp, "")

    # Split and filter for transition-related lines
    for line in result.split('\n'):
        # Show transition names and their conditions
        if ("Transition" in line or
            "bIsAccelerating" in line or
            "Speed" in line or
            "IDLE" in line or
            "CYCLE" in line or
            "Pin" in line or
            "Greater" in line or
            "NOT Boolean" in line or
            "bCanEnterTransition" in line or
            "---" in line):
            log(line)

    log("")
    log("=== STATE MACHINE STRUCTURE ===")
    result = unreal.SLFAutomationLibrary.get_state_machine_structure(bp, "States")
    for line in result.split('\n'):
        log(line)

if __name__ == "__main__":
    main()
