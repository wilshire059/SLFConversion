# fix_animbp_reparent_only.py
# Minimal fix: Only reparent, keep all Blueprint variables
import unreal

def log(msg):
    unreal.log_warning(f"[REPARENT] {msg}")

def main():
    log("=" * 70)
    log("MINIMAL ANIMBP FIX - REPARENT ONLY")
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

    # Step 1: Check current parent
    current_parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
    log(f"Current parent: {current_parent}")

    # Step 2: List current Blueprint variables
    current_vars = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
    log(f"Current Blueprint variables: {len(current_vars)}")
    for v in current_vars:
        log(f"  {v}")

    # Step 3: Reparent to C++ class
    log("")
    log("=== REPARENTING TO C++ CLASS ===")
    if "/Script/SLFConversion" not in str(current_parent):
        success = unreal.SLFAutomationLibrary.reparent_blueprint(bp, cpp_parent_path)
        if success:
            log("[OK] Reparented successfully")
        else:
            log("[ERROR] Failed to reparent")
            return
    else:
        log("[OK] Already reparented to C++")

    # Step 4: Fix Property Access paths (IsGuarding? -> IsGuarding)
    log("")
    log("=== FIXING PROPERTY ACCESS PATHS ===")
    property_fixes = [
        ("IsGuarding?", "IsGuarding"),
    ]

    for old_path, new_path in property_fixes:
        fixed = unreal.SLFAutomationLibrary.fix_property_access_paths(bp, old_path, new_path)
        if fixed > 0:
            log(f"  Fixed '{old_path}' -> '{new_path}': {fixed} occurrences")

    # Step 5: Compile and save
    log("")
    log("=== COMPILING AND SAVING ===")
    success = unreal.SLFAutomationLibrary.compile_and_save(bp)
    if success:
        log("[OK] Compiled and saved successfully")
    else:
        log("[WARN] Compile may have warnings")

    # Step 6: Check for errors
    errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
    if errors:
        log("")
        log("=== COMPILE MESSAGES ===")
        for line in errors.split('\n')[:30]:
            log(line)
    else:
        log("[OK] No compile errors!")

    # Step 7: Final state
    log("")
    log("=== FINAL STATE ===")
    final_parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
    log(f"Final parent: {final_parent}")

    final_vars = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
    log(f"Final Blueprint variables: {len(final_vars)}")
    for v in final_vars:
        log(f"  {v}")

if __name__ == "__main__":
    main()
