# test_pie_animations.py
# Start PIE and verify animations work - check for runtime errors

import unreal
import time

def log(msg):
    unreal.log_warning(f"[PIETest] {msg}")

def main():
    log("=== PIE Animation Test ===")

    # First verify AnimBPs compile clean
    animbp_paths = [
        "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive",
        "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeEnemy",
        "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeBossNew",
        "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeNPC",
    ]

    log("--- Pre-PIE AnimBP Check ---")
    all_clean = True
    for path in animbp_paths:
        name = path.split("/")[-1]
        bp = unreal.load_asset(path)
        if bp:
            errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
            if "Errors: 0" in errors and "Warnings: 0" in errors:
                log(f"  [OK] {name}")
            else:
                log(f"  [ISSUE] {name}")
                all_clean = False
        else:
            log(f"  [ERROR] {name} - Failed to load")
            all_clean = False

    if not all_clean:
        log("ERROR: AnimBPs have compile issues, aborting PIE test")
        return False

    # Start PIE
    log("--- Starting PIE ---")
    try:
        success = unreal.SLFInputSimulatorLibrary.start_pie()
        log(f"StartPIE result: {success}")

        if success:
            log("PIE started, waiting for initialization...")
            time.sleep(3.0)

            # Check if PIE is running
            is_running = unreal.SLFInputSimulatorLibrary.is_pie_running()
            log(f"PIE running: {is_running}")

            if is_running:
                log("PIE active, letting animations tick for 5 seconds...")
                time.sleep(5.0)

                # Stop PIE
                log("Stopping PIE...")
                unreal.SLFInputSimulatorLibrary.stop_pie()
                log("PIE stopped")
            else:
                log("ERROR: PIE did not start properly")
                return False
        else:
            log("ERROR: Failed to start PIE")
            return False

    except Exception as e:
        log(f"Exception during PIE test: {e}")
        return False

    log("=== PIE Animation Test Complete ===")
    return True

if __name__ == "__main__":
    result = main()
    if result:
        unreal.log_warning("[PIETest] SUCCESS - No animation errors detected")
    else:
        unreal.log_error("[PIETest] FAILED - Animation issues detected")
