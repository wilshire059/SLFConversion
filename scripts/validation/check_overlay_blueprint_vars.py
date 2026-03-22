#!/usr/bin/env python3
"""Check overlay state Blueprint variables vs C++ properties"""

import unreal

def log(msg):
    print(f"[OverlayCheck] {msg}")
    unreal.log_warning(f"[OverlayCheck] {msg}")

def main():
    log("=" * 70)
    log("OVERLAY STATE VARIABLE CHECK")
    log("=" * 70)

    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

    bp = unreal.load_asset(animbp_path)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    # Get all Blueprint variables
    vars_list = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
    log(f"\nBlueprint Variables ({len(vars_list)}):")
    for v in vars_list:
        log(f"  - {v}")

    # Get CDO and check overlay state values
    log("\n--- CDO Overlay State Values ---")
    gen_class_path = f"{animbp_path}.ABP_SoulslikeCharacter_Additive_C"
    gen_class = unreal.load_class(None, gen_class_path)

    if gen_class:
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            props = [
                'left_hand_overlay_state',
                'right_hand_overlay_state',
                'active_overlay_state',
                'LeftHandOverlayState',
                'RightHandOverlayState',
                'ActiveOverlayState',
                'LeftHandOverlayState_0',
                'RightHandOverlayState_0',
            ]
            for prop in props:
                try:
                    val = cdo.get_editor_property(prop)
                    log(f"  CDO.{prop} = {val}")
                except Exception as e:
                    pass  # Property doesn't exist

    # Check the E_OverlayState Blueprint enum
    log("\n--- Blueprint E_OverlayState Enum ---")
    enum_path = "/Game/SoulslikeFramework/Enums/E_OverlayState"
    bp_enum = unreal.load_asset(enum_path)
    if bp_enum:
        log(f"Blueprint enum exists: {bp_enum.get_name()}")
        # Try to get enum values
        try:
            names = unreal.SLFAutomationLibrary.get_enum_names(bp_enum)
            log(f"  Enum values: {names}")
        except:
            log("  (Could not get enum values)")
    else:
        log("Blueprint enum does not exist (redirected to C++?)")

    # Check C++ ESLFOverlayState enum
    log("\n--- C++ ESLFOverlayState Enum ---")
    try:
        cpp_enum = unreal.load_object(None, "/Script/SLFConversion.ESLFOverlayState")
        if cpp_enum:
            log(f"C++ enum exists")
    except:
        log("C++ enum check failed")

    log("\n" + "=" * 70)

if __name__ == "__main__":
    main()
