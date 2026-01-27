#!/usr/bin/env python3
"""Check AnimBP variables - both Blueprint and inherited C++ properties"""

import unreal

def log(msg):
    unreal.log_warning(f"[VarCheck] {msg}")

def main():
    log("=" * 70)
    log("ANIMBP VARIABLE CHECK")
    log("=" * 70)

    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

    bp = unreal.load_asset(animbp_path)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    # Get Blueprint variables
    vars_list = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
    log(f"\nBlueprint Variables ({len(vars_list)}):")
    for v in vars_list:
        log(f"  {v}")

    # Get parent class
    parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
    log(f"\nParent Class: {parent}")

    # Check if we can access the C++ properties via CDO
    log("\n--- Checking CDO Properties ---")
    gen_class_path = f"{animbp_path}.ABP_SoulslikeCharacter_Additive_C"
    gen_class = unreal.load_class(None, gen_class_path)

    if gen_class:
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            props_to_check = [
                'bIsBlocking', 'bIsBlocking?',
                'bIsFalling', 'bIsFalling?',
                'LeftHandOverlayState', 'LeftHandOverlayState_0',
                'RightHandOverlayState', 'RightHandOverlayState_0',
                'bIsAccelerating', 'bIsAccelerating?',
                'Speed', 'Direction',
            ]

            for prop in props_to_check:
                try:
                    val = cdo.get_editor_property(prop)
                    log(f"  {prop} = {val}")
                except Exception:
                    pass  # Property doesn't exist

    # Check for "?" suffix variables specifically
    log("\n--- Variables with '?' suffix ---")
    question_vars = [v for v in vars_list if '?' in v]
    if question_vars:
        log(f"  Found {len(question_vars)} variables with '?' suffix:")
        for v in question_vars:
            log(f"    {v}")
        log("\n  [WARNING] These may shadow C++ properties!")
        log("  C++ has: bIsBlocking, bIsFalling, etc.")
        log("  Blueprint has: bIsBlocking?, bIsFalling?, etc.")
        log("  AnimGraph may be reading the WRONG variables!")
    else:
        log("  None found")

    log("\n" + "=" * 70)

if __name__ == "__main__":
    main()
