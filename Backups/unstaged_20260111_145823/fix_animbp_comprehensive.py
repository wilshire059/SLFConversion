# fix_animbp_comprehensive.py - Comprehensive AnimBP fix with verification
import unreal

def log(msg):
    unreal.log_warning(f"[COMPFIX] {msg}")

def main():
    log("=" * 70)
    log("COMPREHENSIVE ANIMBP FIX")
    log("=" * 70)

    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"
    bp = unreal.load_asset(animbp_path)

    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    if not hasattr(unreal, 'SLFAutomationLibrary'):
        log("[ERROR] SLFAutomationLibrary not available")
        return

    # Step 1: Fix all broken transitions
    log("")
    log("=== STEP 1: FIX ALL BROKEN TRANSITIONS ===")
    fixed = unreal.SLFAutomationLibrary.fix_all_broken_transitions(bp)
    log(f"Fixed {fixed} transitions")

    # Step 2: Mark as modified and dirty
    log("")
    log("=== STEP 2: MARKING MODIFIED ===")
    bp.modify(True)
    
    # Step 3: Compile
    log("")
    log("=== STEP 3: COMPILING ===")
    result = unreal.SLFAutomationLibrary.compile_and_save(bp)
    log(f"Compile result: {result}")

    # Step 4: Force save asset explicitly
    log("")
    log("=== STEP 4: FORCE SAVING ===")
    save_result = unreal.EditorAssetLibrary.save_asset(animbp_path, only_if_is_dirty=False)
    log(f"Save result: {save_result}")

    # Step 5: Check errors after save
    log("")
    log("=== STEP 5: CHECKING ERRORS ===")
    errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
    if errors:
        for line in errors.split('\n')[:30]:
            log(line)
    else:
        log("[OK] No compile errors!")

if __name__ == "__main__":
    main()
