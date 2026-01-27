#!/usr/bin/env python3
"""
Full AnimBP diagnostic using C++ SLFAutomationLibrary.
"""

import unreal

def log(msg):
    unreal.log_warning(f"[DIAG] {msg}")

ANIMBP_PATH = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

def main():
    log("=" * 70)
    log("FULL ANIMBP DIAGNOSTIC")
    log("=" * 70)

    bp = unreal.load_asset(ANIMBP_PATH)
    if not bp:
        log("ERROR: Could not load AnimBP")
        return

    if not hasattr(unreal, 'SLFAutomationLibrary'):
        log("ERROR: SLFAutomationLibrary not available")
        return

    # Run full diagnostic
    log("")
    diag = unreal.SLFAutomationLibrary.diagnose_anim_bp(bp)

    # Print all diagnostic output
    for line in diag.split('\n'):
        log(line)

if __name__ == "__main__":
    main()
