# diagnose_animbp_state.py
# Diagnose the current state of the AnimBP including parent class and variables
import unreal

def log(msg):
    print(msg)
    unreal.log_warning(f"[DIAG] {msg}")

def main():
    log("=" * 70)
    log("DIAGNOSING ANIMBP STATE")
    log("=" * 70)

    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

    bp = unreal.load_asset(animbp_path)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    # Check generated class and parent
    log("")
    log("=== CLASS HIERARCHY ===")
    gen_class = bp.generated_class()
    if gen_class:
        log(f"Generated Class: {gen_class.get_name()}")
        log(f"Generated Class Path: {gen_class.get_path_name()}")

        # Get parent class
        parent = gen_class.get_super_class()
        while parent:
            log(f"  -> Parent: {parent.get_name()} ({parent.get_path_name()})")
            parent = parent.get_super_class()
    else:
        log("Generated Class: None")

    # Check compile status
    log("")
    log("=== COMPILE STATUS ===")
    if hasattr(unreal, 'SLFAutomationLibrary'):
        errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
        if errors:
            for line in errors.split('\n')[:20]:
                log(line)
        else:
            log("No compile errors")

    # Check available variables via SLFAutomationLibrary
    log("")
    log("=== BLUEPRINT VARIABLES (via GetBlueprintVariables) ===")
    if hasattr(unreal, 'SLFAutomationLibrary'):
        variables = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
        for var in variables[:30]:  # Show first 30
            log(f"  {var}")
        if len(variables) > 30:
            log(f"  ... and {len(variables) - 30} more")
    else:
        log("SLFAutomationLibrary not available")

    log("")
    log("=== END DIAGNOSIS ===")

if __name__ == "__main__":
    main()
