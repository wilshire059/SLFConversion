# test_anim_cpp_logging.py
# Test if C++ AnimInstance NativeUpdateAnimation is being called
# by looking for [ANIM_CPP] log entries during PIE

import unreal
import time

def log(msg):
    unreal.log_warning(f"[AnimTest] {msg}")

def main():
    log("=" * 60)
    log("Testing C++ Animation Logging")
    log("=" * 60)
    log("")
    log("EXPECTED OUTPUT:")
    log("  [ANIM_CPP] NativeUpdateAnimation FIRST CALL - C++ AnimInstance is active!")
    log("  [ANIM_CPP] Speed=X.X Falling=0 Blocking=0 RightOverlay=X")
    log("")
    log("If you see these logs, C++ AnimInstance is working.")
    log("If NOT, the AnimBP is NOT reparented to C++ class.")
    log("")

    # First verify the AnimBP parent
    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"
    bp = unreal.load_asset(animbp_path)
    if bp:
        parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
        log(f"AnimBP parent: {parent}")
        if "SLFConversion" in parent:
            log("  [OK] AnimBP is reparented to C++ class!")
        else:
            log("  [FAIL] AnimBP is NOT reparented to C++ class!")
            log("  Run reparent_animbps_to_cpp.py first!")
            return

    log("")
    log("Starting PIE for 10 seconds...")

    success = unreal.SLFInputSimulatorLibrary.start_pie()
    if not success:
        log("[ERROR] Failed to start PIE")
        return

    log("PIE started - waiting for animation logs...")
    time.sleep(10.0)

    log("Stopping PIE...")
    unreal.SLFInputSimulatorLibrary.stop_pie()

    log("=" * 60)
    log("CHECK LOGS ABOVE for [ANIM_CPP] entries!")
    log("=" * 60)
    log("")
    log("SUCCESS CRITERIA:")
    log("  - See '[ANIM_CPP] NativeUpdateAnimation FIRST CALL' message")
    log("  - See '[ANIM_CPP] Speed=X.X' periodic messages")
    log("")
    log("If no [ANIM_CPP] logs appear, the C++ NativeUpdateAnimation is not being called.")

if __name__ == "__main__":
    main()
