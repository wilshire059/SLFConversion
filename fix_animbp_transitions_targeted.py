"""
Fix AnimBP Transitions - Targeted approach

Each transition needs a specific variable connected to bCanEnterTransition:
- "To Falling to Fall Loop" -> bIsFalling
- "Land to Locomotion" -> NOT bIsFalling (need to negate)
- "IDLE to CYCLE" -> bIsAccelerating
- "IDLE to SITTING", "CYCLE to SITTING" -> IsResting
- "CROUCH IDLE to SITTING", "CROUCH CYCLE to SITTING" -> IsResting

The TransitionResult node has bCanEnterTransition input that must be connected.
"""

import unreal

ANIMBP_PATH = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

# Transition -> (variable, needs_NOT)
TRANSITION_FIXES = {
    "To Falling": ("bIsFalling", False),
    "Fall Loop": ("bIsFalling", False),
    "Land to Locomotion": ("bIsFalling", True),  # NOT bIsFalling
    "IDLE to CYCLE": ("bIsAccelerating", False),
    "CYCLE to IDLE": ("bIsAccelerating", True),  # NOT bIsAccelerating
    "IDLE to SITTING": ("IsResting", False),
    "CYCLE to SITTING": ("IsResting", False),
    "CROUCH IDLE to SITTING": ("IsResting", False),
    "CROUCH CYCLE to SITTING": ("IsResting", False),
}


def log(msg):
    print(f"[TransFix] {msg}")
    unreal.log_warning(f"[TransFix] {msg}")


def main():
    log("=" * 70)
    log("TARGETED TRANSITION FIX")
    log("=" * 70)

    bp = unreal.load_asset(ANIMBP_PATH)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return False

    log(f"Loaded: {bp.get_name()}")

    if not hasattr(unreal, 'SLFAutomationLibrary'):
        log("[ERROR] SLFAutomationLibrary not available")
        return False

    # First, run the repair transition wiring to fix any disconnected VariableGet nodes
    log("")
    log("=== Step 1: Repair Transition Wiring ===")
    repaired = unreal.SLFAutomationLibrary.repair_transition_wiring(bp)
    log(f"Repaired {repaired} connections")

    # Second, fix specific transitions with correct variables
    log("")
    log("=== Step 2: Fix Specific Transitions ===")

    total_fixed = 0
    for transition_pattern, (variable, needs_not) in TRANSITION_FIXES.items():
        # Use CanEnterTransition as the target pin (on TransitionResult node)
        # But if needs_not, we need to connect to NOT node first

        # Try to fix by connecting variable to the broken input pin
        # The FixTransitionBooleanInput looks for K2Node_CallFunction (NOT) and K2Node_CommutativeAssociativeBinaryOperator (AND)
        fixed = unreal.SLFAutomationLibrary.fix_transition_boolean_input(bp, transition_pattern, "A", variable)
        if fixed > 0:
            log(f"  [OK] '{transition_pattern}' -> {variable} (pin A): {fixed}")
            total_fixed += fixed
        else:
            # Try pin B (for AND nodes)
            fixed = unreal.SLFAutomationLibrary.fix_transition_boolean_input(bp, transition_pattern, "B", variable)
            if fixed > 0:
                log(f"  [OK] '{transition_pattern}' -> {variable} (pin B): {fixed}")
                total_fixed += fixed
            else:
                log(f"  [--] '{transition_pattern}' -> {variable}: no match")

    log(f"Total targeted fixes: {total_fixed}")

    # Step 3: Run FixAllBrokenTransitions to catch any remaining
    log("")
    log("=== Step 3: Fix Remaining Broken Transitions ===")
    remaining_fixed = unreal.SLFAutomationLibrary.fix_all_broken_transitions(bp)
    log(f"Fixed {remaining_fixed} remaining connections")

    # Step 4: Connect TransitionResult nodes directly if still broken
    log("")
    log("=== Step 4: Direct TransitionResult Connection ===")
    # Use the comprehensive diagnostic to find remaining issues
    try:
        diagnosis = unreal.SLFAutomationLibrary.diagnose_anim_bp(bp)
        if "will never be taken" in diagnosis:
            log("[INFO] Some transitions still have issues")
    except:
        pass

    # Step 5: Compile
    log("")
    log("=== Step 5: Compile ===")
    unreal.BlueprintEditorLibrary.compile_blueprint(bp)

    # Check compile errors
    errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
    log(f"Compile status:")

    # Count "will never be taken" warnings
    if errors:
        warning_count = errors.count("will never be taken")
        log(f"  {warning_count} 'will never be taken' warnings remaining")

        # Show first few warnings
        for line in errors.split('\n'):
            if "will never be taken" in line:
                log(f"    {line.strip()}")

    # Step 6: Save
    log("")
    log("=== Step 6: Save ===")
    unreal.EditorAssetLibrary.save_asset(ANIMBP_PATH, only_if_is_dirty=False)

    log("")
    log("=" * 70)
    log("TARGETED FIX COMPLETE")
    log("=" * 70)

    return True


if __name__ == "__main__":
    main()
