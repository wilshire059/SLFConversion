# reparent_animbp.py
# Reparent AnimBP to C++ AnimInstance class
# Run AFTER rename_animbp_variables.py

import unreal

def log(msg):
    unreal.log_warning(f"[REPARENT] {msg}")

def main():
    log("=" * 70)
    log("REPARENTING ANIMBP TO C++ CLASS")
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
    log("")
    log("=== CURRENT STATE ===")
    current_parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
    log(f"Current parent: {current_parent}")

    # Step 2: Check variables before reparenting
    current_vars = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
    log(f"Blueprint variables: {len(current_vars)}")
    for v in current_vars:
        log(f"  {v}")

    # Step 3: Reparent to C++ class
    log("")
    log("=== REPARENTING ===")
    if "/Script/SLFConversion" not in str(current_parent):
        success = unreal.SLFAutomationLibrary.reparent_blueprint(bp, cpp_parent_path)
        if success:
            log("[OK] Reparented to C++ class")
        else:
            log("[ERROR] Reparent failed")
            return
    else:
        log("[OK] Already reparented to C++")

    # Step 4: Clear EventGraph (keep AnimGraph)
    log("")
    log("=== CLEARING EVENT GRAPH ===")
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

    # Step 7: Check for errors
    errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
    if errors:
        log("")
        log("=== COMPILE STATUS ===")
        for line in errors.split('\n')[:30]:
            log(line)
    else:
        log("[OK] No compile errors!")

    # Step 8: Final state
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
