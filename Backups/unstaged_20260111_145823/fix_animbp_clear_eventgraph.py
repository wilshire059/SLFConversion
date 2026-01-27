# fix_animbp_clear_eventgraph.py
# Clear EventGraph and remove Blueprint functions
import unreal

def log(msg):
    unreal.log_warning(f"[CLEAR] {msg}")

def main():
    log("=" * 70)
    log("CLEARING EVENTGRAPH AND FUNCTIONS")
    log("=" * 70)

    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

    bp = unreal.load_asset(animbp_path)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    if not hasattr(unreal, 'SLFAutomationLibrary'):
        log("[ERROR] SLFAutomationLibrary not available")
        return

    # Step 1: Clear EventGraph
    log("")
    log("=== CLEARING EVENT GRAPH ===")
    cleared = unreal.SLFAutomationLibrary.clear_event_graphs(bp)
    log(f"Cleared {cleared} nodes from EventGraph")

    # Step 2: Get and remove Blueprint functions
    log("")
    log("=== REMOVING BLUEPRINT FUNCTIONS ===")
    funcs = unreal.SLFAutomationLibrary.get_blueprint_functions(bp)
    log(f"Found {len(funcs)} Blueprint functions")
    for func_name in funcs:
        log(f"  {func_name}")
    
    for func_name in funcs:
        # Skip Construction Script
        if "ConstructionScript" in func_name:
            log(f"  [SKIP] {func_name} (Construction Script)")
            continue
        success = unreal.SLFAutomationLibrary.remove_function(bp, func_name)
        if success:
            log(f"  [OK] Removed {func_name}")
        else:
            log(f"  [--] Could not remove {func_name}")

    # Step 3: Compile and save
    log("")
    log("=== COMPILING AND SAVING ===")
    success = unreal.SLFAutomationLibrary.compile_and_save(bp)
    if success:
        log("[OK] Compiled and saved")
    else:
        log("[WARN] Compile may have issues")

    # Step 4: Check for errors
    errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
    if errors:
        log("")
        log("=== COMPILE STATUS ===")
        for line in errors.split('\n')[:20]:
            log(line)
    else:
        log("[OK] No compile errors!")

    # Step 5: Final state
    log("")
    log("=== FINAL STATE ===")
    final_funcs = unreal.SLFAutomationLibrary.get_blueprint_functions(bp)
    log(f"Remaining Blueprint functions: {len(final_funcs)}")
    for f in final_funcs:
        log(f"  {f}")

if __name__ == "__main__":
    main()
