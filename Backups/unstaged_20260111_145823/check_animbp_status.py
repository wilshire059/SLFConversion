# check_animbp_status.py
# Check AnimBP compile status

import unreal

def log(msg):
    unreal.log_warning(f"[CHECK] {msg}")

def main():
    log("=" * 70)
    log("ANIMBP COMPILE CHECK")
    log("=" * 70)

    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

    bp = unreal.load_asset(animbp_path)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    log(f"AnimBP: {animbp_path}")
    parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
    log(f"Parent Class: {parent}")

    # Check compile status
    log("")
    log("Compile status:")
    errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
    for line in errors.split('\n'):
        if line.strip():
            log(f"  {line}")

    log("")
    log("=" * 70)

if __name__ == "__main__":
    main()
