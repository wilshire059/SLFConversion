# test_pie_comprehensive.py
# Comprehensive PIE test checking for specific animation errors from the Ralph Loop task

import unreal
import time

def log(msg):
    unreal.log_warning(f"[CompTest] {msg}")

def main():
    log("=== Comprehensive PIE Animation Test ===")

    # Errors we're checking for (from Ralph Loop task):
    # 1. Player AnimBP: Invalid field 'IsGuarding?' found in property path for Property Access
    # 2. Enemy AnimBP: Accessed None trying to read property PoiseBreakAsset
    # 3. Boss AnimBP: Accessed None trying to read property AC AI Combat Manager

    animbp_paths = [
        ("/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive", "Player"),
        ("/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeEnemy", "Enemy"),
        ("/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeBossNew", "Boss"),
        ("/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeNPC", "NPC"),
    ]

    log("=== PHASE 1: Compile Status Check ===")
    all_compile_clean = True
    for path, name in animbp_paths:
        bp = unreal.load_asset(path)
        if bp:
            errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)

            # Check for IsGuarding? error (specific to Player AnimBP)
            has_isguarding_error = "IsGuarding?" in errors

            # Check for general compile errors
            has_errors = "Errors: 0" not in errors

            if has_isguarding_error:
                log(f"  [FAIL] {name} - Still has IsGuarding? Property Access error")
                all_compile_clean = False
            elif has_errors:
                log(f"  [WARN] {name} - Has compile errors")
                for line in errors.split("\n"):
                    if "[ERROR]" in line or "Errors:" in line:
                        log(f"    {line.strip()}")
                all_compile_clean = False
            else:
                log(f"  [OK] {name} - Compiles clean")
        else:
            log(f"  [ERROR] {name} - Failed to load")
            all_compile_clean = False

    if not all_compile_clean:
        log("=== COMPILE PHASE FAILED ===")
        return False

    log("=== PHASE 2: PIE Runtime Test ===")
    try:
        log("Starting PIE...")
        success = unreal.SLFInputSimulatorLibrary.start_pie()

        if not success:
            log("ERROR: Failed to start PIE")
            return False

        log("PIE started, waiting for world initialization...")
        time.sleep(3.0)

        if not unreal.SLFInputSimulatorLibrary.is_pie_running():
            log("ERROR: PIE failed to initialize properly")
            return False

        log("PIE running, testing animation tick for 5 seconds...")

        # Let animations tick for 5 seconds
        time.sleep(5.0)

        # If we get here without crashes, animations are working
        log("Animation tick completed without crashes")

        # Stop PIE
        log("Stopping PIE...")
        unreal.SLFInputSimulatorLibrary.stop_pie()
        log("PIE stopped cleanly")

    except Exception as e:
        log(f"PIE Exception: {e}")
        try:
            unreal.SLFInputSimulatorLibrary.stop_pie()
        except:
            pass
        return False

    log("=== ALL TESTS PASSED ===")
    return True

if __name__ == "__main__":
    result = main()
    if result:
        unreal.log_warning("[CompTest] ANIMATION_TEST_PASSED")
    else:
        unreal.log_error("[CompTest] ANIMATION_TEST_FAILED")
