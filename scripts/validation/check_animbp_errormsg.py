# check_animbp_errormsg.py
# Quick check of current AnimBP ErrorMsg status

import unreal

def log(msg):
    print(f"[AnimBP] {msg}")
    unreal.log_warning(f"[AnimBP] {msg}")

ANIMBP_PATH = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

def main():
    log("=" * 60)
    log("CHECKING ANIMBP ERRORMSG STATUS")
    log("=" * 60)

    bp = unreal.load_asset(ANIMBP_PATH)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    log(f"Loaded: {bp.get_name()}")

    if not hasattr(unreal, 'SLFAutomationLibrary'):
        log("[ERROR] SLFAutomationLibrary not available")
        return

    # Run diagnosis
    diagnosis = unreal.SLFAutomationLibrary.diagnose_anim_bp(bp)

    # Check for "will never be taken"
    if "will never be taken" in diagnosis:
        count = diagnosis.count("will never be taken")
        log(f"[WARNING] Found {count} transitions with 'will never be taken' warnings!")

        # Extract the transition names
        lines = diagnosis.split('\n')
        for line in lines:
            if "will never be taken" in line:
                log(f"  - {line.strip()}")
    else:
        log("[SUCCESS] No 'will never be taken' warnings in diagnosis!")

    # Also check compile errors
    errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
    log(f"Compile Status: {errors}")

    # Check Blueprint variables
    variables = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
    log(f"Blueprint Variables ({len(variables)}):")
    for v in variables:
        log(f"  - {v}")

    log("[DONE]")

if __name__ == "__main__":
    main()
