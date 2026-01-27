# fix_animbp_minimal.py
# Minimal fix: Only reparent and fix Property Access paths
# DO NOT remove or rename Blueprint variables
import unreal

def log(msg):
    unreal.log_warning(f"[MINFIX] {msg}")

def main():
    log("=" * 70)
    log("MINIMAL ANIMBP FIX - REPARENT AND FIX PROPERTY ACCESS ONLY")
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

    # Step 2: Reparent to C++ class
    log("")
    log("=== REPARENTING TO C++ CLASS ===")
    success = unreal.SLFAutomationLibrary.reparent_blueprint(bp, cpp_parent_path)
    if success:
        log("[OK] Reparented successfully")
    else:
        log("[ERROR] Failed to reparent")
        return

    # Step 3: Fix Property Access paths ONLY (don't remove/rename variables)
    log("")
    log("=== FIXING PROPERTY ACCESS PATHS ONLY ===")
    property_fixes = [
        ("IsGuarding?", "IsGuarding"),
    ]

    total_fixed = 0
    for old_name, new_name in property_fixes:
        log(f"Fixing '{old_name}' -> '{new_name}'...")
        fixed = unreal.SLFAutomationLibrary.fix_property_access_paths(bp, old_name, new_name)
        log(f"  Fixed {fixed} occurrences")
        total_fixed += fixed

    log(f"Total fixed: {total_fixed}")

    # Step 4: Compile and save
    log("")
    log("=== COMPILING AND SAVING ===")
    success = unreal.SLFAutomationLibrary.compile_and_save(bp)
    if success:
        log("[OK] Compiled and saved successfully")
    else:
        log("[WARN] Compile may have warnings")

    # Step 5: Check for errors
    errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
    if errors:
        log("")
        log("=== COMPILE MESSAGES ===")
        for line in errors.split('\n')[:30]:
            log(line)
    else:
        log("[OK] No compile errors!")

    # Step 6: Final state
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
