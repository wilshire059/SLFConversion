#!/usr/bin/env python3
"""Diagnose BlendListByEnum nodes in AnimBP."""

import unreal

def log(msg):
    unreal.log_warning(f"[DIAG] {msg}")

ANIMBP_PATH = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

def main():
    log("=" * 70)
    log("DIAGNOSING BLENDLISTBYENUM NODES")
    log("=" * 70)

    bp = unreal.load_asset(ANIMBP_PATH)
    if not bp:
        log("ERROR: Could not load AnimBP")
        return

    # Use C++ automation to diagnose AnimGraph
    result = unreal.SLFAutomationLibrary.diagnose_anim_graph_nodes(bp)
    for line in result.split('\n'):
        log(line)

if __name__ == "__main__":
    main()
