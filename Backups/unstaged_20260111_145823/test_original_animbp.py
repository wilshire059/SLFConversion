# test_original_animbp.py
# Test if original AnimBP produces working animations

import unreal
import time

def log(msg):
    unreal.log_warning(f"[TestOriginal] {msg}")

def main():
    log("=" * 60)
    log("Testing Original AnimBP (before C++ reparenting)")
    log("=" * 60)

    # Verify AnimBP parent
    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"
    bp = unreal.load_asset(animbp_path)
    parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
    log(f"AnimBP Parent: {parent}")

    if "ABP_SoulslikeCharacter_Additive" in parent:
        log("[WARNING] AnimBP is reparented to C++!")
    else:
        log("[OK] AnimBP uses original Engine.AnimInstance parent")

    # Start PIE
    log("")
    log("Starting PIE for 5 seconds...")
    success = unreal.SLFInputSimulatorLibrary.start_pie()
    if not success:
        log("[ERROR] Failed to start PIE")
        return

    log("PIE started - animations should be playing...")
    time.sleep(5.0)

    log("Stopping PIE...")
    unreal.SLFInputSimulatorLibrary.stop_pie()

    log("")
    log("=" * 60)
    log("If character was NOT frozen (T-pose), original AnimBP works!")
    log("The issue is C++ reparenting breaks the AnimGraph.")
    log("=" * 60)

if __name__ == "__main__":
    main()
