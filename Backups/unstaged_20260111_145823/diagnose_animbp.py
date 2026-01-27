# diagnose_animbp.py
# Run AnimBP diagnostic to find exact state machine structure

import unreal

def log(msg):
    unreal.log_warning(f"[DIAG] {msg}")

def main():
    log("=" * 70)
    log("ANIMBP STATE MACHINE DIAGNOSTIC")
    log("=" * 70)

    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

    bp = unreal.load_asset(animbp_path)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    log(f"Loaded: {animbp_path}")
    log("")

    # Run the diagnostic
    result = unreal.SLFAutomationLibrary.diagnose_anim_bp(bp)

    # Print each line
    for line in result.split('\n'):
        log(line)

    log("")
    log("=" * 70)
    log("Now getting state machine structures...")
    log("=" * 70)

    # Get Locomotion state machine
    log("")
    log("LOCOMOTION STATE MACHINE:")
    loco_result = unreal.SLFAutomationLibrary.get_state_machine_structure(bp, "Locomotion")
    for line in loco_result.split('\n'):
        log(line)

    # Get States state machine
    log("")
    log("STATES STATE MACHINE:")
    states_result = unreal.SLFAutomationLibrary.get_state_machine_structure(bp, "States")
    for line in states_result.split('\n'):
        log(line)

if __name__ == "__main__":
    main()
