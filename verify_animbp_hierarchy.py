#!/usr/bin/env python3
"""Verify AnimBP class hierarchy using C++."""

import unreal

def log(msg):
    unreal.log_warning(f"[VERIFY] {msg}")

ANIMBP_PATH = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

def main():
    log("=" * 70)
    log("VERIFYING ANIMBP CLASS HIERARCHY")
    log("=" * 70)

    if not hasattr(unreal, 'SLFAutomationLibrary'):
        log("ERROR: SLFAutomationLibrary not available")
        return

    bp = unreal.load_asset(ANIMBP_PATH)
    if not bp:
        log("ERROR: Could not load AnimBP")
        return

    # Use C++ to get class hierarchy
    result = unreal.SLFAutomationLibrary.get_class_hierarchy(bp)
    log(result)

    # Also check NativeUpdateAnimation implementation
    log("")
    log("=== CHECKING NATIVEUPDATEANIMATION ===")

    gen_class = bp.generated_class()
    if gen_class:
        # Get CDO
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            # Check if we have EquipmentManager cache
            try:
                equip_mgr = cdo.get_editor_property("equipment_manager")
                log(f"EquipmentManager cached: {equip_mgr}")
            except:
                pass

            # Check overlay state property types
            log("")
            log("Property type checks:")
            try:
                left = cdo.get_editor_property("LeftHandOverlayState")
                log(f"LeftHandOverlayState type: {type(left)}")
                log(f"LeftHandOverlayState value: {left}")
            except Exception as e:
                log(f"LeftHandOverlayState error: {e}")

            try:
                right = cdo.get_editor_property("RightHandOverlayState")
                log(f"RightHandOverlayState type: {type(right)}")
                log(f"RightHandOverlayState value: {right}")
            except Exception as e:
                log(f"RightHandOverlayState error: {e}")

if __name__ == "__main__":
    main()
