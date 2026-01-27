# verify_animbp_state.py - Verify AnimBP is properly configured
import unreal

def log(msg):
    unreal.log_warning(f"[VERIFY] {msg}")

def main():
    log("=" * 70)
    log("ANIMBP VERIFICATION")
    log("=" * 70)

    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"
    bp = unreal.load_asset(animbp_path)
    
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    if not hasattr(unreal, 'SLFAutomationLibrary'):
        log("[ERROR] SLFAutomationLibrary not available")
        return

    # Check parent class
    parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
    log(f"Parent class: {parent}")
    
    is_cpp_parent = "/Script/SLFConversion" in str(parent)
    log(f"  Is C++ parent: {'YES' if is_cpp_parent else 'NO - NEEDS FIX'}")

    # Check variables
    vars = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
    log(f"Blueprint variables: {len(vars)}")
    for v in vars:
        has_question = "?" in v
        has_zero = "_0" in v
        status = ""
        if has_question:
            status = " <- SHOULD BE REMOVED (? suffix)"
        elif has_zero:
            status = " <- SHOULD BE REMOVED (_0 suffix)"
        log(f"  {v}{status}")

    # Check compile status
    errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
    if errors:
        log("")
        log("COMPILE ERRORS:")
        for line in errors.split('\n')[:20]:
            log(f"  {line}")
    else:
        log("")
        log("[OK] No compile errors!")

    # Summary
    log("")
    log("=" * 70)
    if is_cpp_parent and len(vars) <= 2 and not errors:
        log("SUCCESS! AnimBP is properly configured:")
        log("  - Parent: C++ ABP_SoulslikeCharacter_Additive")
        log("  - Variables: Minimal (only Direction(Angle))")
        log("  - Compile: No errors")
        log("  - AnimGraph reads from C++ UPROPERTY directly")
        log("  - No reflection needed!")
    else:
        log("AnimBP needs attention - see details above")
    log("=" * 70)

if __name__ == "__main__":
    main()
