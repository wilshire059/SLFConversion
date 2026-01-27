# test_anim_tick_detailed.py
# Test animation tick in PIE and check for animation-related logs/errors

import unreal
import time

def log(msg):
    unreal.log_warning(f"[AnimTick] {msg}")

def main():
    log("=" * 60)
    log("Animation Tick PIE Test")
    log("=" * 60)

    # First check AnimBP compile status
    animbp_paths = [
        "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive",
        "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeEnemy",
        "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeBossNew",
        "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeNPC",
    ]

    log("--- Pre-PIE AnimBP Check ---")
    for path in animbp_paths:
        name = path.split("/")[-1]
        bp = unreal.load_asset(path)
        if bp:
            # Get parent class
            parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
            errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
            is_clean = "Errors: 0" in errors and "Warnings: 0" in errors
            status = "[OK]" if is_clean else "[ISSUE]"
            log(f"  {status} {name} - Parent: {parent}")
        else:
            log(f"  [ERROR] {name} - Failed to load")

    # Start PIE
    log("--- Starting PIE ---")
    try:
        success = unreal.SLFInputSimulatorLibrary.start_pie()
        if not success:
            log("[ERROR] Failed to start PIE")
            return False

        log("PIE started, waiting for initialization...")
        time.sleep(2.0)

        if not unreal.SLFInputSimulatorLibrary.is_pie_running():
            log("[ERROR] PIE not running")
            return False

        log("PIE running - letting animations tick for 5 seconds...")
        log("(Check logs for [AnimBP DEBUG] entries)")

        # Wait for animation updates to log
        time.sleep(5.0)

        log("--- Stopping PIE ---")
        unreal.SLFInputSimulatorLibrary.stop_pie()
        log("PIE stopped")

    except Exception as e:
        log(f"[ERROR] PIE Exception: {e}")
        try:
            unreal.SLFInputSimulatorLibrary.stop_pie()
        except:
            pass
        return False

    log("=" * 60)
    log("Animation Tick Test Complete")
    log("=" * 60)
    return True

if __name__ == "__main__":
    result = main()
    if result:
        unreal.log_warning("[AnimTick] TEST COMPLETE")
    else:
        unreal.log_error("[AnimTick] TEST FAILED")
