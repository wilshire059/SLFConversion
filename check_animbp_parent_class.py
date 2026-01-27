#!/usr/bin/env python3
"""Check AnimBP parent class to verify C++ reparenting."""

import unreal

def log(msg):
    unreal.log_warning(f"[CHECK] {msg}")

ANIMBP_PATH = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

def main():
    log("=" * 70)
    log("CHECKING ANIMBP PARENT CLASS")
    log("=" * 70)

    bp = unreal.load_asset(ANIMBP_PATH)
    if not bp:
        log("ERROR: Could not load AnimBP")
        return

    log(f"AnimBP: {bp.get_name()}")
    log(f"AnimBP class: {bp.get_class().get_name()}")

    # Get parent class property
    try:
        parent_class = bp.get_editor_property("parent_class")
        if parent_class:
            log(f"Parent class: {parent_class.get_name()}")
            log(f"Parent class path: {parent_class.get_path_name()}")

            # Check if it's a C++ class
            path = parent_class.get_path_name()
            if "/Script/" in path:
                log(">> Parent is C++ class (GOOD)")
            else:
                log(">> Parent is Blueprint class - NOT reparented to C++!")
        else:
            log("Parent class: None")
    except Exception as e:
        log(f"Error getting parent_class: {e}")

    # Get generated class info
    gen_class = bp.generated_class()
    if gen_class:
        log("")
        log(f"Generated class: {gen_class.get_name()}")
        log(f"Generated class path: {gen_class.get_path_name()}")

        # Get CDO and check overlay state values
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            log("")
            log("CDO overlay state values:")
            try:
                log(f"  LeftHandOverlayState: {cdo.get_editor_property('LeftHandOverlayState')}")
            except:
                try:
                    log(f"  left_hand_overlay_state: {cdo.get_editor_property('left_hand_overlay_state')}")
                except:
                    log("  LeftHandOverlayState: NOT FOUND")

            try:
                log(f"  RightHandOverlayState: {cdo.get_editor_property('RightHandOverlayState')}")
            except:
                try:
                    log(f"  right_hand_overlay_state: {cdo.get_editor_property('right_hand_overlay_state')}")
                except:
                    log("  RightHandOverlayState: NOT FOUND")

    # Check expected C++ class
    log("")
    log("=== EXPECTED C++ PARENT ===")
    cpp_class = unreal.load_class(None, "/Script/SLFConversion.ABP_SoulslikeCharacter_Additive")
    if cpp_class:
        log(f"C++ AnimInstance class found: {cpp_class.get_name()}")

        # Compare
        if parent_class and cpp_class:
            if parent_class == cpp_class:
                log(">> AnimBP IS correctly parented to C++ class")
            else:
                log(f">> AnimBP is NOT parented to C++ class!")
                log(f"   Current: {parent_class.get_name()}")
                log(f"   Expected: {cpp_class.get_name()}")
    else:
        log("C++ AnimInstance class NOT FOUND")

if __name__ == "__main__":
    main()
