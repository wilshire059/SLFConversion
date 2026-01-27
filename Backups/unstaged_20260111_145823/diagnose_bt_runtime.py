"""
Diagnose BT at runtime during PIE
This runs a quick PIE test and captures BT debug output
"""
import unreal
import time

def log(msg):
    unreal.log_warning(f"[BT_RUNTIME] {msg}")

def main():
    log("=" * 70)
    log("DIAGNOSING BT AT RUNTIME")
    log("=" * 70)

    # Load the level
    level_path = "/Game/SoulslikeFramework/Maps/L_Demo_Showcase"

    log(f"Loading level: {level_path}")

    # Start PIE
    log("Starting PIE session...")

    # Use automation library to run PIE test
    pie_result = unreal.SLFAutomationLibrary.run_pie_test(5.0)  # 5 second test

    log("")
    log("PIE Test Result:")
    for line in pie_result.split('\n'):
        log(line)

    log("")
    log("=" * 70)
    log("DONE")
    log("=" * 70)

if __name__ == "__main__":
    main()
