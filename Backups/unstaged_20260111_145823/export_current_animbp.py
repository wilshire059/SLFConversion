# export_current_animbp.py
# Export the current AnimBP state to see its structure

import unreal

def log(msg):
    unreal.log_warning(f"[EXPORT] {msg}")

def main():
    log("=" * 70)
    log("ANIMBP STATE EXPORT - CURRENT STATE")
    log("=" * 70)

    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

    bp = unreal.load_asset(animbp_path)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    log(f"AnimBP loaded: {bp.get_name()}")

    # Get class hierarchy
    gen_class = bp.generated_class()
    if gen_class:
        log(f"Generated Class: {gen_class.get_name()}")

    # Use C++ diagnostic
    log("")
    log("Running C++ diagnostics...")

    if hasattr(unreal, 'SLFAutomationLibrary'):
        # Full diagnostic
        result = unreal.SLFAutomationLibrary.diagnose_anim_bp(bp)
        log("")
        log("FULL DIAGNOSTIC:")
        for line in result.split('\n'):
            log(line)

        # State machine structures
        log("")
        log("LOCOMOTION STATE MACHINE STRUCTURE:")
        loco = unreal.SLFAutomationLibrary.get_state_machine_structure(bp, "Locomotion")
        for line in loco.split('\n'):
            log(line)

        # Export to JSON
        json_output_path = "C:/scripts/SLFConversion/migration_cache/animbp_current_state.json"
        json_result = unreal.SLFAutomationLibrary.export_blueprint_state(bp, json_output_path)
        log(f"JSON export saved to: {json_output_path}")
    else:
        log("[WARN] SLFAutomationLibrary not available")

    log("")
    log("=" * 70)
    log("EXPORT COMPLETE - Check migration_cache/ for outputs")
    log("=" * 70)

if __name__ == "__main__":
    main()
