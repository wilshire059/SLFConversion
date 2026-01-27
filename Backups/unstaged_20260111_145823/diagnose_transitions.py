# diagnose_transitions.py - Diagnose state machine transitions
import unreal

def log(msg):
    unreal.log_warning(f"[TRANS] {msg}")

def main():
    log("=" * 70)
    log("DIAGNOSING STATE MACHINE TRANSITIONS")
    log("=" * 70)

    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"
    bp = unreal.load_asset(animbp_path)

    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    if hasattr(unreal, 'SLFAutomationLibrary'):
        # Get transition diagnosis
        diagnosis = unreal.SLFAutomationLibrary.diagnose_state_machine_transitions(bp)
        if diagnosis:
            for line in diagnosis.split('\n')[:100]:
                log(line)
        else:
            log("No diagnosis available - function may not exist")

if __name__ == "__main__":
    main()
