# fix_animbp_complete.py
# Complete AnimBP fix: Replace variable refs, clear EventGraph, remove duplicates
# NO REFLECTION NEEDED - AnimGraph reads directly from C++ properties
import unreal

def log(msg):
    unreal.log_warning(f"[COMPLETE] {msg}")

def main():
    log("=" * 70)
    log("COMPLETE ANIMBP FIX - NO REFLECTION APPROACH")
    log("=" * 70)

    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"
    cpp_parent_path = "/Script/SLFConversion.ABP_SoulslikeCharacter_Additive"

    bp = unreal.load_asset(animbp_path)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    if not hasattr(unreal, 'SLFAutomationLibrary'):
        log("[ERROR] SLFAutomationLibrary not available")
        return

    # Step 1: Check current state
    log("")
    log("=== CURRENT STATE ===")
    current_parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
    log(f"Current parent: {current_parent}")

    current_vars = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
    log(f"Current Blueprint variables: {len(current_vars)}")
    for v in current_vars:
        log(f"  {v}")

    # Step 2: Ensure reparented to C++ class
    if "ABP_SoulslikeCharacter_Additive" not in str(current_parent) or "/Script/" not in str(current_parent):
        log("")
        log("=== REPARENTING TO C++ CLASS ===")
        success = unreal.SLFAutomationLibrary.reparent_blueprint(bp, cpp_parent_path)
        if success:
            log("[OK] Reparented successfully")
        else:
            log("[ERROR] Failed to reparent")
            return
    else:
        log("[OK] Already reparented to C++ class")

    # Step 3: Replace variable references BEFORE renaming
    # This updates AnimGraph nodes to use C++ property names
    log("")
    log("=== REPLACING VARIABLE REFERENCES IN ANIMGRAPH ===")
    variable_mappings = [
        ("bIsAccelerating?", "bIsAccelerating"),
        ("bIsBlocking?", "bIsBlocking"),
        ("bIsFalling?", "bIsFalling"),
        ("IsResting?", "IsResting"),
        # Also handle _0 suffix variants if they exist
        ("bIsAccelerating_0", "bIsAccelerating"),
        ("LeftHandOverlayState_0", "LeftHandOverlayState"),
        ("RightHandOverlayState_0", "RightHandOverlayState"),
    ]

    total_replaced = 0
    for old_name, new_name in variable_mappings:
        replaced = unreal.SLFAutomationLibrary.replace_variable_references(bp, old_name, new_name)
        if replaced > 0:
            log(f"  Replaced '{old_name}' -> '{new_name}': {replaced} occurrences")
            total_replaced += replaced

    log(f"Total variable references replaced: {total_replaced}")

    # Step 4: Fix Property Access paths
    log("")
    log("=== FIXING PROPERTY ACCESS PATHS ===")
    property_fixes = [
        ("IsGuarding?", "IsGuarding"),
        ("bIsAccelerating?", "bIsAccelerating"),
        ("bIsBlocking?", "bIsBlocking"),
        ("bIsFalling?", "bIsFalling"),
        ("IsResting?", "IsResting"),
    ]

    total_fixed = 0
    for old_path, new_path in property_fixes:
        fixed = unreal.SLFAutomationLibrary.fix_property_access_paths(bp, old_path, new_path)
        if fixed > 0:
            log(f"  Fixed '{old_path}' -> '{new_path}': {fixed} occurrences")
            total_fixed += fixed

    log(f"Total Property Access paths fixed: {total_fixed}")

    # Step 5: Clear EventGraph (C++ NativeUpdateAnimation handles all logic)
    log("")
    log("=== CLEARING EVENT GRAPH ===")
    cleared = unreal.SLFAutomationLibrary.clear_event_graphs(bp)
    log(f"EventGraph cleared: {cleared}")

    # Step 6: Remove duplicate/obsolete Blueprint variables
    log("")
    log("=== REMOVING OBSOLETE BLUEPRINT VARIABLES ===")
    vars_to_remove = [
        "bIsAccelerating?",
        "bIsBlocking?",
        "bIsFalling?",
        "IsResting?",
        "bIsAccelerating_0",
        "LeftHandOverlayState_0",
        "RightHandOverlayState_0",
    ]

    for var_name in vars_to_remove:
        success = unreal.SLFAutomationLibrary.remove_variable(bp, var_name)
        if success:
            log(f"  [OK] Removed '{var_name}'")

    # Step 7: Remove duplicate Blueprint functions (C++ implements them)
    log("")
    log("=== REMOVING BLUEPRINT FUNCTIONS ===")
    # The AnimBP may have Blueprint functions that duplicate C++ implementations
    funcs_to_remove = [
        "GetOwnerVelocity",
        "GetOwnerRotation",
    ]

    for func_name in funcs_to_remove:
        success = unreal.SLFAutomationLibrary.remove_function(bp, func_name)
        if success:
            log(f"  [OK] Removed function '{func_name}'")

    # Step 8: Compile and save
    log("")
    log("=== COMPILING AND SAVING ===")
    success = unreal.SLFAutomationLibrary.compile_and_save(bp)
    if success:
        log("[OK] Compiled and saved successfully")
    else:
        log("[WARN] Compile may have warnings")

    # Step 9: Check for errors
    errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
    if errors:
        log("")
        log("=== COMPILE MESSAGES ===")
        for line in errors.split('\n')[:30]:
            log(line)
    else:
        log("[OK] No compile errors!")

    # Step 10: Final state
    log("")
    log("=== FINAL STATE ===")
    final_parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
    log(f"Final parent: {final_parent}")

    final_vars = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
    log(f"Final Blueprint variables: {len(final_vars)}")
    for v in final_vars:
        log(f"  {v}")

    log("")
    log("=" * 70)
    log("COMPLETE! AnimGraph now reads directly from C++ UPROPERTY variables.")
    log("No reflection needed - NativeUpdateAnimation sets the properties.")
    log("=" * 70)

if __name__ == "__main__":
    main()
