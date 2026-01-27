# fix_animbp_clear_function_graphs.py
# Clear function graphs that reference old Blueprint variables

import unreal

def log(msg):
    unreal.log_warning(f"[FixFuncGraphs] {msg}")

def main():
    log("=" * 60)
    log("Clearing Function Graphs from AnimBP")
    log("=" * 60)

    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"
    bp = unreal.load_asset(animbp_path)
    if not bp:
        log(f"[ERROR] Failed to load AnimBP")
        return

    # Get current status
    parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
    log(f"AnimBP Parent: {parent}")

    # Get function names before clearing
    funcs = unreal.SLFAutomationLibrary.get_blueprint_functions(bp)
    log(f"Functions to clear: {len(funcs)}")
    for f in funcs:
        log(f"  - {f}")

    # Clear function graphs
    log("")
    log("Clearing function graphs...")
    cleared = unreal.SLFAutomationLibrary.clear_function_graphs(bp)
    log(f"Cleared {cleared} nodes from function graphs")

    # Save
    unreal.EditorAssetLibrary.save_asset(animbp_path)
    log("[OK] Saved AnimBP")

    # Reload and check
    bp = unreal.load_asset(animbp_path)
    funcs = unreal.SLFAutomationLibrary.get_blueprint_functions(bp)
    log(f"Remaining functions: {len(funcs)}")

    errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
    log("")
    log("Compile Status:")
    for line in errors.split('\n')[:20]:
        if line.strip():
            log(f"  {line}")

    log("=" * 60)

if __name__ == "__main__":
    main()
