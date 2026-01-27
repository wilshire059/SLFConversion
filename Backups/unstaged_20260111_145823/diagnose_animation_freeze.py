# diagnose_animation_freeze.py
# Diagnose why animations are frozen

import unreal
import time

def log(msg):
    unreal.log_warning(f"[DiagAnim] {msg}")

def main():
    log("=" * 70)
    log("ANIMATION FREEZE DIAGNOSIS")
    log("=" * 70)

    # 1. Check AnimBP compile status and parent
    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"
    bp = unreal.load_asset(animbp_path)
    if not bp:
        log(f"[ERROR] Failed to load AnimBP")
        return

    parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
    log(f"AnimBP Parent: {parent}")

    errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
    log(f"Compile Status:")
    for line in errors.split('\n')[:10]:
        log(f"  {line}")

    # 2. Run AnimGraph diagnosis
    log("")
    log("=== AnimGraph Diagnosis ===")
    diagnosis = unreal.SLFAutomationLibrary.diagnose_anim_graph_nodes(bp)
    # Print first 50 lines
    lines = diagnosis.split('\n')[:50]
    for line in lines:
        log(f"  {line}")

    # 3. Start PIE and check for errors
    log("")
    log("=== PIE Test ===")
    log("Starting PIE...")

    success = unreal.SLFInputSimulatorLibrary.start_pie()
    if not success:
        log("[ERROR] Failed to start PIE")
        return

    log("PIE started, letting it run for 5 seconds...")
    time.sleep(5.0)

    log("Stopping PIE...")
    unreal.SLFInputSimulatorLibrary.stop_pie()

    log("")
    log("=== Check Log for These Issues ===")
    log("  - 'Accessed None' errors")
    log("  - 'Property Access' errors")
    log("  - 'IsGuarding?' errors")
    log("  - 'ExecuteUbergraph' errors")
    log("=" * 70)

if __name__ == "__main__":
    main()
