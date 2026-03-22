#!/usr/bin/env python3
"""Simple check of AnimBP parent class using C++ automation."""

import unreal

def log(msg):
    unreal.log_warning(f"[PARENT] {msg}")

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

    # Use C++ automation to get parent class
    parent_class = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
    log(f"Parent class: {parent_class}")

    if "/Script/SLFConversion" in parent_class:
        log(">> AnimBP IS reparented to C++ (SLFConversion module)")
    elif "/Script/" in parent_class:
        log(f">> AnimBP parent is C++ but NOT SLFConversion: {parent_class}")
    else:
        log(">> AnimBP is NOT reparented to C++ - still using Blueprint/default parent!")

    # Expected C++ parent
    log("")
    log("Expected parent: /Script/SLFConversion.ABP_SoulslikeCharacter_Additive")

if __name__ == "__main__":
    main()
