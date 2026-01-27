# inspect_all_transitions.py
# Inspect ALL transition graphs

import unreal

def log(msg):
    print(f"[Inspect] {msg}")
    unreal.log_warning(f"[Inspect] {msg}")

def main():
    log("=" * 70)
    log("INSPECTING ALL TRANSITION GRAPHS")
    log("=" * 70)

    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

    bp = unreal.load_asset(animbp_path)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    if not hasattr(unreal, 'SLFAutomationLibrary'):
        log("[ERROR] SLFAutomationLibrary not available")
        return

    # Inspect all transitions (empty keyword)
    log("\n=== ALL TRANSITIONS ===")
    result = unreal.SLFAutomationLibrary.inspect_transition_graph(bp, "")
    for line in result.split('\n'):
        log(line)

    # Also get state machine structure
    log("\n=== STATE MACHINE STRUCTURE ===")
    result2 = unreal.SLFAutomationLibrary.get_state_machine_structure(bp)
    for line in result2.split('\n'):
        log(line)

if __name__ == "__main__":
    main()
