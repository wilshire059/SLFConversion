# check_animbp_parent_now.py
import unreal

def log(msg):
    print(msg)
    unreal.log_warning(f"[PARENT] {msg}")

def main():
    log("=" * 70)
    log("CHECKING ANIMBP PARENT CLASS")
    log("=" * 70)

    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

    bp = unreal.load_asset(animbp_path)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    log(f"AnimBP: {bp.get_name()}")

    # Get parent class
    parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
    log(f"Parent Class: {parent}")

    # Check if it's our C++ class
    expected = "/Script/SLFConversion.ABP_SoulslikeCharacter_Additive"
    if expected in parent:
        log("[OK] AnimBP is parented to our C++ AnimInstance!")
    else:
        log("[ERROR] AnimBP is NOT parented to our C++ class!")
        log(f"Expected: {expected}")
        log("NativeUpdateAnimation() is NOT running!")

    # Check functions
    log("")
    funcs = unreal.SLFAutomationLibrary.get_blueprint_functions(bp)
    log(f"Blueprint functions: {len(funcs) if funcs else 0}")
    if funcs:
        for f in funcs:
            log(f"  - {f}")

    # Check variables
    vars_list = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
    log(f"Blueprint variables: {len(vars_list) if vars_list else 0}")

if __name__ == "__main__":
    main()
