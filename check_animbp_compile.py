"""
Quick check to verify AnimBP loads and compiles without enum errors.
"""

import unreal

ANIMBP_PATH = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("CHECK ANIMBP COMPILE STATUS")
    unreal.log_warning("=" * 70)

    animbp = unreal.load_asset(ANIMBP_PATH)
    if not animbp:
        unreal.log_warning("[ERROR] Could not load AnimBP")
        return

    unreal.log_warning(f"Loaded: {animbp.get_name()}")

    # Get compile status
    errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(animbp)

    if errors and len(errors) > 0:
        unreal.log_warning(f"[ERRORS] Compile errors found:")
        unreal.log_warning(errors)
    else:
        unreal.log_warning("[SUCCESS] No compile errors!")

    unreal.log_warning("=" * 70)

if __name__ == "__main__":
    main()
