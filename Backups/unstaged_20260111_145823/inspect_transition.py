# inspect_transition.py
# Use C++ to inspect transition graphs - looking for Speed comparison

import unreal

def log(msg):
    unreal.log_warning(f"[INSPECT] {msg}")

def main():
    log("=" * 70)
    log("INSPECTING ANIMBP TRANSITIONS IN DETAIL")
    log("=" * 70)

    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

    bp = unreal.load_asset(animbp_path)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    if hasattr(unreal, 'SLFAutomationLibrary'):
        # Inspect transitions containing "Speed" or "float" (the comparison)
        log("")
        log("TRANSITIONS WITH 'float' (Speed comparison):")
        result1 = unreal.SLFAutomationLibrary.inspect_transition_graph(bp, "float")
        for line in result1.split('\n'):
            log(line)

        log("")
        log("=" * 70)
        log("ALL TRANSITION GRAPHS:")
        log("=" * 70)
        # Show all transitions to see the full picture
        result2 = unreal.SLFAutomationLibrary.inspect_transition_graph(bp, "")
        for line in result2.split('\n'):
            log(line)

    else:
        log("[WARN] SLFAutomationLibrary not available")

    log("")
    log("=" * 70)
    log("INSPECTION COMPLETE")
    log("=" * 70)

if __name__ == "__main__":
    main()
