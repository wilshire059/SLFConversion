#!/usr/bin/env python3
"""
Remove Blueprint variables with '?' suffix from AnimBP.

These variables shadow the C++ properties and cause the AnimGraph
to read stale values (always false/0) because C++ updates the
non-'?' properties but AnimGraph reads the '?' suffixed Blueprint vars.

Variables to remove:
- bIsAccelerating?
- bIsBlocking?
- bIsFalling?
- IsResting?
- Direction(Angle)
- RightHandOverlayState_0
- LeftHandOverlayState_0
"""

import unreal

def log(msg):
    unreal.log_warning(f"[RemoveVars] {msg}")

def main():
    log("=" * 70)
    log("REMOVING QUESTION MARK SUFFIX BLUEPRINT VARIABLES")
    log("=" * 70)

    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

    bp = unreal.load_asset(animbp_path)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    # List current variables
    vars_before = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
    log(f"Variables before: {len(vars_before)}")
    for v in vars_before:
        log(f"  {v}")

    # Variables to remove - these shadow C++ properties
    vars_to_remove = [
        "bIsAccelerating?",
        "bIsBlocking?",
        "bIsFalling?",
        "IsResting?",
        "Direction(Angle)",  # C++ has 'Direction'
        "RightHandOverlayState_0",
        "LeftHandOverlayState_0",
    ]

    log(f"\nRemoving {len(vars_to_remove)} variables...")

    removed = 0
    for var_name in vars_to_remove:
        result = unreal.SLFAutomationLibrary.remove_variable(bp, var_name)
        if result:
            log(f"  [OK] Removed: {var_name}")
            removed += 1
        else:
            log(f"  [SKIP] Not found or could not remove: {var_name}")

    log(f"\nRemoved {removed} variables")

    # Compile and save
    log("\nCompiling and saving...")
    if removed > 0:
        success = unreal.SLFAutomationLibrary.compile_and_save(bp)
        log(f"Compile and save: {'SUCCESS' if success else 'FAILED'}")
    else:
        log("No changes to save")

    # List variables after
    bp = unreal.load_asset(animbp_path)  # Reload
    vars_after = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
    log(f"\nVariables after: {len(vars_after)}")
    for v in vars_after:
        log(f"  {v}")

    # Check compile status
    errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
    log(f"\nCompile status:")
    for line in errors.split('\n')[:10]:
        if line.strip():
            log(f"  {line}")

    log("\n" + "=" * 70)
    log("NOTE: The AnimGraph nodes that were reading from these variables")
    log("will now read from the C++ properties with the same names (without ?)")
    log("=" * 70)

if __name__ == "__main__":
    main()
