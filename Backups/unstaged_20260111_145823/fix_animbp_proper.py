# fix_animbp_proper.py
# Proper AnimBP fix: reparent, fix paths, clear EventGraph only (NOT functions)
import unreal

def log(msg):
    unreal.log_warning(f"[FIX] {msg}")

def main():
    log("=" * 70)
    log("PROPER ANIMBP FIX")
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

    # Step 1: Current state
    log("")
    log("=== CURRENT STATE ===")
    current_parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
    log(f"Parent: {current_parent}")

    # Step 2: Reparent to C++
    log("")
    log("=== REPARENTING ===")
    if "/Script/SLFConversion" not in str(current_parent):
        success = unreal.SLFAutomationLibrary.reparent_blueprint(bp, cpp_parent_path)
        if success:
            log("[OK] Reparented to C++")
        else:
            log("[ERROR] Reparent failed")
            return
    else:
        log("[OK] Already C++ parent")

    # Step 3: Fix Property Access paths
    log("")
    log("=== FIXING PROPERTY ACCESS PATHS ===")
    fixes = [("IsGuarding?", "IsGuarding")]
    for old_path, new_path in fixes:
        fixed = unreal.SLFAutomationLibrary.fix_property_access_paths(bp, old_path, new_path)
        if fixed > 0:
            log(f"  Fixed '{old_path}' -> '{new_path}': {fixed}")

    # Step 4: Clear EventGraph ONLY (do NOT touch functions - AnimGraph needs them)
    log("")
    log("=== CLEARING EVENT GRAPH ONLY ===")
    cleared = unreal.SLFAutomationLibrary.clear_event_graphs(bp)
    log(f"Cleared {cleared} nodes from EventGraph")

    # Step 5: Compile and save
    log("")
    log("=== COMPILING AND SAVING ===")
    success = unreal.SLFAutomationLibrary.compile_and_save(bp)
    if success:
        log("[OK] Compiled and saved")
    else:
        log("[WARN] Compile may have issues")

    # Step 6: Force save
    save_result = unreal.EditorAssetLibrary.save_asset(animbp_path, only_if_is_dirty=False)
    log(f"Save result: {save_result}")

    # Step 7: Check errors
    errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
    if errors:
        log("")
        log("=== COMPILE STATUS ===")
        for line in errors.split('\n')[:30]:
            log(line)
    else:
        log("[OK] No compile errors!")

if __name__ == "__main__":
    main()
