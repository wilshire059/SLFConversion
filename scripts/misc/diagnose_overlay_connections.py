# diagnose_overlay_connections.py
# Detailed diagnostic of AnimBP blend node connections

import unreal

def log(msg):
    print(f"[Diag] {msg}")
    unreal.log_warning(f"[Diag] {msg}")

ANIMBP_PATH = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

def main():
    log("=" * 70)
    log("DETAILED BLEND NODE CONNECTION DIAGNOSTIC")
    log("=" * 70)

    # Load AnimBP
    animbp = unreal.load_asset(ANIMBP_PATH)
    if not animbp:
        log(f"ERROR: Could not load AnimBP")
        return

    log(f"Loaded: {animbp.get_name()}")

    # Get detailed AnimGraph state using C++ function
    try:
        state = unreal.SLFAutomationLibrary.diagnose_anim_graph_nodes(animbp)
        log("\n" + state)
    except Exception as e:
        log(f"diagnose_anim_graph_nodes error: {e}")

    # Also check the actual runtime values
    log("\n--- Checking C++ AnimInstance CDO ---")
    try:
        cpp_class = unreal.load_class(None, "/Script/SLFConversion.ABP_SoulslikeCharacter_Additive")
        if cpp_class:
            cdo = unreal.get_default_object(cpp_class)
            if cdo:
                for prop in ['left_hand_overlay_state', 'right_hand_overlay_state', 'active_overlay_state']:
                    try:
                        val = cdo.get_editor_property(prop)
                        log(f"  {prop}: {val}")
                    except:
                        pass
    except Exception as e:
        log(f"CDO check error: {e}")

if __name__ == "__main__":
    main()
