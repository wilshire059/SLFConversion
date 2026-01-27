# test_animation_final.py
# Final animation test - verify component references are set and animations work

import unreal
import time

def log(msg):
    unreal.log_warning(f"[ANIMFIX] {msg}")

def main():
    log("=" * 70)
    log("FINAL ANIMATION FIX VERIFICATION")
    log("=" * 70)

    # 1. Check AnimBP status
    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"
    bp = unreal.load_asset(animbp_path)
    parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
    log(f"AnimBP Parent: {parent}")

    errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
    log("AnimBP Compile:")
    for line in errors.split('\n')[:5]:
        if line.strip():
            log(f"  {line}")

    # 2. Start PIE
    log("")
    log("Starting PIE...")
    success = unreal.SLFInputSimulatorLibrary.start_pie()
    if not success:
        log("[ERROR] Failed to start PIE")
        return

    log("PIE started - waiting 8 seconds for animation initialization...")

    # Log every 2 seconds
    for i in range(4):
        time.sleep(2.0)
        log(f"  {(i+1)*2} seconds elapsed...")

    log("")
    log("Stopping PIE...")
    unreal.SLFInputSimulatorLibrary.stop_pie()

    log("")
    log("=" * 70)
    log("VERIFICATION COMPLETE")
    log("")
    log("Check the log for:")
    log("  1. Component references being set (ActionMgrProp, CombatMgrProp, EquipMgrProp)")
    log("  2. No 'Accessed None' errors during PIE")
    log("  3. Animation state changes being logged")
    log("")
    log("If character is still frozen, the AnimBP needs to be reparented to C++")
    log("and the duplicate variable issue must be resolved differently.")
    log("=" * 70)

if __name__ == "__main__":
    main()
