# test_anim_detailed.py
# Detailed PIE test to capture any animation-related errors

import unreal
import time

def log(msg):
    unreal.log_warning(f"[AnimTest] {msg}")

def main():
    log("=" * 70)
    log("DETAILED ANIMATION TEST")
    log("=" * 70)

    # Check AnimBP status
    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"
    bp = unreal.load_asset(animbp_path)
    parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
    log(f"AnimBP Parent: {parent}")

    errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
    log("Compile Status:")
    for line in errors.split('\n')[:10]:
        if line.strip():
            log(f"  {line}")

    # Start PIE and capture output
    log("")
    log("Starting PIE...")
    success = unreal.SLFInputSimulatorLibrary.start_pie()
    if not success:
        log("[ERROR] Failed to start PIE")
        return

    log("PIE started - waiting 10 seconds for animations to initialize...")

    # Wait and observe
    for i in range(10):
        time.sleep(1.0)
        log(f"  Second {i+1}...")

    log("")
    log("Stopping PIE...")
    unreal.SLFInputSimulatorLibrary.stop_pie()

    log("")
    log("=" * 70)
    log("LOOK FOR THESE IN THE FULL LOG:")
    log("  - 'Accessed None' errors")
    log("  - 'Animation' or 'Anim' errors")
    log("  - 'StateMachine' errors")
    log("  - 'Property Access' errors")
    log("  - 'IsGuarding?' issues")
    log("=" * 70)

if __name__ == "__main__":
    main()
