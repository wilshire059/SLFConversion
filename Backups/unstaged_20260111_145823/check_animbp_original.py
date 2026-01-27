# check_animbp_original.py
# Check the original AnimBP status (after restore from backup)

import unreal

def log(msg):
    unreal.log_warning(f"[CheckOriginal] {msg}")

def main():
    log("=" * 60)
    log("Checking Original AnimBP Status")
    log("=" * 60)

    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"
    bp = unreal.load_asset(animbp_path)
    if not bp:
        log(f"[ERROR] Failed to load AnimBP")
        return

    parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
    log(f"AnimBP Parent: {parent}")

    errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
    log("Compile Status:")
    for line in errors.split('\n')[:20]:
        if line.strip():
            log(f"  {line}")

    # Get blueprint variables
    bp_vars = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
    log(f"")
    log(f"Blueprint Variables: {len(bp_vars)}")
    for v in bp_vars:
        log(f"  - {v}")

    log("=" * 60)

if __name__ == "__main__":
    main()
