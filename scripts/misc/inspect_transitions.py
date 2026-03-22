# inspect_transitions.py
# Inspect transition graphs to understand what needs to be fixed

import unreal

def log(msg):
    print(f"[Inspect] {msg}")
    unreal.log_warning(f"[Inspect] {msg}")

def main():
    log("=" * 70)
    log("INSPECTING TRANSITION GRAPHS")
    log("=" * 70)

    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

    bp = unreal.load_asset(animbp_path)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    if not hasattr(unreal, 'SLFAutomationLibrary'):
        log("[ERROR] SLFAutomationLibrary not available")
        return

    # Inspect key transitions
    transitions_to_check = [
        "IDLE to CYCLE",
        "CYCLE to IDLE",
        "TransitionNode_0",
        "TransitionNode_1",
    ]

    for keyword in transitions_to_check:
        log(f"\n=== Inspecting: {keyword} ===")
        result = unreal.SLFAutomationLibrary.inspect_transition_graph(bp, keyword)
        for line in result.split('\n'):
            if line.strip():
                log(f"  {line}")

if __name__ == "__main__":
    main()
