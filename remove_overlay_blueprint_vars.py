#!/usr/bin/env python3
"""Remove Blueprint overlay state variables that shadow C++ properties"""

import unreal

def log(msg):
    print(f"[RemoveVars] {msg}")
    unreal.log_warning(f"[RemoveVars] {msg}")

def main():
    log("=" * 70)
    log("REMOVING SHADOWING BLUEPRINT VARIABLES")
    log("=" * 70)

    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

    bp = unreal.load_asset(animbp_path)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    # Get current Blueprint variables
    vars_before = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
    log(f"\nVariables BEFORE: {vars_before}")

    # Variables to remove (these shadow C++ properties)
    vars_to_remove = [
        "LeftHandOverlayState_0",
        "RightHandOverlayState_0",
    ]

    for var_name in vars_to_remove:
        if var_name in vars_before:
            log(f"\nRemoving variable: {var_name}")
            result = unreal.SLFAutomationLibrary.remove_variable(bp, var_name)
            log(f"  Result: {'SUCCESS' if result else 'FAILED'}")
        else:
            log(f"\nVariable not found: {var_name}")

    # Get updated Blueprint variables
    vars_after = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
    log(f"\nVariables AFTER: {vars_after}")

    # Save the Blueprint
    log("\nSaving Blueprint...")
    save_result = unreal.EditorAssetLibrary.save_asset(animbp_path)
    log(f"Save result: {'SUCCESS' if save_result else 'FAILED'}")

    log("\n" + "=" * 70)

if __name__ == "__main__":
    main()
