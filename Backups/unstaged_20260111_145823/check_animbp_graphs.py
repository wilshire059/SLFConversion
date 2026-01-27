# check_animbp_graphs.py
# Check what graphs/functions exist in the AnimBP

import unreal

def log(msg):
    unreal.log_warning(f"[CHECK] {msg}")

def main():
    log("=" * 70)
    log("CHECKING ANIMBP GRAPHS AND FUNCTIONS")
    log("=" * 70)

    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

    bp = unreal.load_asset(animbp_path)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    if not hasattr(unreal, 'SLFAutomationLibrary'):
        log("[ERROR] SLFAutomationLibrary not available")
        return

    # Check parent
    log("")
    log("=== PARENT CLASS ===")
    parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
    log(f"Parent: {parent}")

    # Get all functions
    log("")
    log("=== BLUEPRINT FUNCTIONS ===")
    funcs = unreal.SLFAutomationLibrary.get_blueprint_functions(bp)
    log(f"Count: {len(funcs)}")
    for f in funcs:
        log(f"  {f}")

    # Get Blueprint variables
    log("")
    log("=== BLUEPRINT VARIABLES ===")
    bp_vars = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
    log(f"Count: {len(bp_vars)}")
    for v in bp_vars:
        log(f"  {v}")

    # Get event dispatchers
    log("")
    log("=== EVENT DISPATCHERS ===")
    dispatchers = unreal.SLFAutomationLibrary.get_blueprint_event_dispatchers(bp)
    log(f"Count: {len(dispatchers)}")
    for d in dispatchers:
        log(f"  {d}")

    # Run full diagnostic
    log("")
    log("=== FULL ANIMGRAPH DIAGNOSTIC ===")
    diagnostic = unreal.SLFAutomationLibrary.diagnose_anim_bp(bp)
    for line in diagnostic.split('\n')[:100]:
        log(line)

if __name__ == "__main__":
    main()
