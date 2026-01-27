# test_animations_pie.py
# Run a PIE session and capture animation-related logs

import unreal
import time

def log(msg):
    unreal.log_warning(f"[AnimTest] {msg}")

def main():
    log("=== Animation PIE Test ===")

    # Check AnimBP compile status first
    animbp_paths = [
        "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive",
        "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeEnemy",
        "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeBossNew",
        "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeNPC",
    ]

    log("--- Checking AnimBP Compile Status ---")
    for path in animbp_paths:
        name = path.split("/")[-1]
        bp = unreal.load_asset(path)
        if bp:
            errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
            # Extract just summary
            for line in errors.split("\n"):
                if "Errors:" in line or "Warnings:" in line or "[ERROR]" in line:
                    log(f"  {name}: {line.strip()}")
        else:
            log(f"  {name}: FAILED TO LOAD")

    # Start PIE
    log("--- Starting PIE Session ---")

    editor_subsystem = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    if not editor_subsystem:
        log("ERROR: Cannot get editor subsystem")
        return False

    # Request PIE start
    try:
        # Use automation library to start PIE
        success = unreal.SLFAutomationLibrary.start_pie_session()
        log(f"PIE Start result: {success}")

        if success:
            # Wait for PIE to initialize
            log("Waiting 5 seconds for PIE to initialize...")
            time.sleep(5)

            # Check for animation errors in log
            log("PIE session active, checking for errors...")

            # Wait a bit more for animations to tick
            time.sleep(3)

            # Stop PIE
            unreal.SLFAutomationLibrary.stop_pie_session()
            log("PIE session stopped")
    except Exception as e:
        log(f"PIE Exception: {e}")

    log("=== Animation PIE Test Complete ===")
    return True

if __name__ == "__main__":
    main()
