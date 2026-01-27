# check_property_access.py
# Check Property Access paths and variable bindings in AnimBP

import unreal

def log(msg):
    print(msg)
    unreal.log_warning(f"[PA] {msg}")

def main():
    bp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

    bp = unreal.load_asset(bp_path)
    if not bp:
        log("[ERROR] Cannot load ABP")
        return

    log("=" * 70)
    log("CHECKING PROPERTY ACCESS IN ABP_SoulslikeCharacter_Additive")
    log("=" * 70)

    # Get the generated class
    gen_class = bp.generated_class()
    log(f"Generated Class: {gen_class}")

    if gen_class:
        parent = gen_class.get_super_class()
        log(f"Parent Class: {parent}")

    # Get Property Access paths using SLFAutomationLibrary
    log("")
    log("=== Property Access Paths ===")
    if hasattr(unreal, 'SLFAutomationLibrary'):
        paths = unreal.SLFAutomationLibrary.get_property_access_paths(bp)
        if paths:
            for p in paths:
                log(f"  {p}")
        else:
            log("  (no paths found)")
    else:
        log("  SLFAutomationLibrary not available")

    # List all variables on the AnimBP
    log("")
    log("=== AnimBP Variables ===")
    if hasattr(unreal, 'SLFAutomationLibrary'):
        vars_list = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
        if vars_list:
            for v in vars_list:
                log(f"  {v}")
        else:
            log("  (no variables found)")

    # Check if C++ properties exist that AnimGraph might reference
    log("")
    log("=== Checking C++ Properties for Animation ===")
    key_props = [
        ("Speed", "double"),
        ("Direction", "double"),
        ("bIsAccelerating", "bool"),
        ("bIsFalling", "bool"),
        ("bIsBlocking", "bool"),
        ("IsResting", "bool"),
        ("IsCrouched", "bool"),
        ("Velocity", "FVector"),
        ("Velocity2D", "FVector"),
    ]

    if gen_class:
        cdo = unreal.get_default_object(gen_class)
        for prop_name, expected_type in key_props:
            try:
                val = cdo.get_editor_property(prop_name.lower())
                log(f"  [OK] {prop_name} = {val}")
            except:
                try:
                    # Try with original casing
                    val = cdo.get_editor_property(prop_name)
                    log(f"  [OK] {prop_name} = {val}")
                except Exception as e:
                    log(f"  [--] {prop_name} NOT ACCESSIBLE")

    log("")
    log("=== Check Complete ===")

if __name__ == "__main__":
    main()
