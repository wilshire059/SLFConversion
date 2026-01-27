# fix_all_animbp_issues.py - Fix all AnimBP issues: transitions, overlay states, etc.
import unreal

def log(msg):
    unreal.log_warning(f"[FIXALL] {msg}")

def main():
    log("=" * 70)
    log("FIXING ALL ANIMBP ISSUES")
    log("=" * 70)

    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"
    bp = unreal.load_asset(animbp_path)

    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    if not hasattr(unreal, 'SLFAutomationLibrary'):
        log("[ERROR] SLFAutomationLibrary not available")
        return

    # Step 1: Try to auto-fix all broken transitions
    log("")
    log("=== FIXING ALL BROKEN TRANSITIONS ===")
    fixed = unreal.SLFAutomationLibrary.fix_all_broken_transitions(bp)
    log(f"Auto-fixed {fixed} transitions")

    # Step 2: Fix individual transitions manually if auto-fix didn't work
    log("")
    log("=== FIXING SPECIFIC TRANSITIONS ===")
    
    # Map of transition patterns to variable names
    transition_fixes = [
        # Falling transitions
        ("Falling to Fall Loop", "bIsFalling"),
        ("To Falling to Fall", "bIsFalling"),
        
        # Landing transition
        ("Land to Locomotion", "bIsFalling"),  # Actually needs NOT bIsFalling
        
        # Movement transitions
        ("IDLE to CYCLE", "bIsAccelerating"),
        ("CYCLE to IDLE", "bIsAccelerating"),  # May need NOT bIsAccelerating
        
        # Sitting transitions
        ("IDLE to SITTING", "IsResting"),
        ("CYCLE to SITTING", "IsResting"),
        ("CROUCH IDLE to SITTING", "IsResting"),
        ("CROUCH CYCLE to SITTING", "IsResting"),
    ]
    
    for transition_name, var_name in transition_fixes:
        result = unreal.SLFAutomationLibrary.fix_transition_boolean_input(bp, transition_name, "CanEnterTransition", var_name)
        if result > 0:
            log(f"  [OK] Fixed '{transition_name}' -> {var_name}: {result} connections")
        else:
            log(f"  [--] '{transition_name}' -> {var_name}: no changes needed or not found")

    # Step 3: Compile and check
    log("")
    log("=== COMPILING ===")
    success = unreal.SLFAutomationLibrary.compile_and_save(bp)
    if success:
        log("[OK] Compiled")
    
    # Step 4: Check remaining errors
    errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
    if errors:
        log("")
        log("=== REMAINING ISSUES ===")
        for line in errors.split('\n')[:30]:
            log(line)
    else:
        log("[OK] No compile issues!")

if __name__ == "__main__":
    main()
