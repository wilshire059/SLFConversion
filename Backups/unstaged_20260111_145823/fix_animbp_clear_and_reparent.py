# fix_animbp_clear_and_reparent.py
# Clear AnimBP EventGraph (keeping variables and AnimGraph), then reparent to C++
# This removes the problematic Property Access nodes

import unreal

def log(msg):
    unreal.log_warning(f"[FixAnimBP] {msg}")

def main():
    log("=" * 60)
    log("Fix AnimBP: Clear EventGraph and Reparent to C++")
    log("=" * 60)

    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"
    bp = unreal.load_asset(animbp_path)
    if not bp:
        log(f"[ERROR] Failed to load AnimBP")
        return

    # Check current status
    parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
    log(f"Current Parent: {parent}")

    errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
    log(f"Current Status: {errors.split(chr(10))[4] if len(errors.split(chr(10))) > 4 else 'Unknown'}")

    # Step 1: Clear EventGraph and function graphs (keeps variables and AnimGraph)
    log("")
    log("Step 1: Clearing EventGraph and function graphs...")
    success = unreal.SLFAutomationLibrary.clear_graphs_keep_variables_no_compile(bp)
    if success:
        log("  [OK] Graphs cleared (variables and AnimGraph preserved)")
    else:
        log("  [FAIL] Failed to clear graphs")
        return

    # Step 2: Save before reparent
    unreal.EditorAssetLibrary.save_asset(animbp_path)
    log("  [OK] Saved AnimBP")

    # Step 3: Reparent to C++ class
    log("")
    log("Step 2: Reparenting to C++ AnimInstance...")
    cpp_class = "/Script/SLFConversion.ABP_SoulslikeCharacter_Additive"
    success = unreal.SLFAutomationLibrary.reparent_blueprint(bp, cpp_class)
    if success:
        log(f"  [OK] Reparented to {cpp_class}")
    else:
        log("  [FAIL] Reparent failed")
        return

    # Step 4: Save after reparent
    unreal.EditorAssetLibrary.save_asset(animbp_path)
    log("  [OK] Saved AnimBP")

    # Step 5: Verify
    log("")
    log("Step 3: Verifying...")
    bp = unreal.load_asset(animbp_path)
    parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
    log(f"  New Parent: {parent}")

    errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
    log(f"  Compile Status:")
    for line in errors.split('\n')[:10]:
        if line.strip():
            log(f"    {line}")

    log("")
    log("=" * 60)
    log("Done! The AnimBP now uses C++ NativeUpdateAnimation.")
    log("Run PIE test to verify [ANIM_CPP] logs appear.")
    log("=" * 60)

if __name__ == "__main__":
    main()
