#!/usr/bin/env python
"""
Clear Vendor NPC EventGraph using C++ SLFAutomationLibrary
"""

import unreal

def clear_vendor():
    print("=" * 60)
    print("CLEAR VENDOR NPC VIA C++ AUTOMATION")
    print("=" * 60)

    # Load the vendor NPC Blueprint
    vendor_bp_path = "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseVendor"
    vendor_bp = unreal.load_asset(vendor_bp_path)

    if not vendor_bp:
        print("ERROR: Could not load vendor NPC Blueprint")
        return

    print("Loaded: " + vendor_bp.get_name())

    # Call C++ automation to clear event graphs
    print("")
    print("=== Calling SLFAutomationLibrary.clear_graphs_keep_variables ===")

    result = unreal.SLFAutomationLibrary.clear_graphs_keep_variables(vendor_bp)
    print("Result: " + str(result))

    if result:
        print("SUCCESS: EventGraphs cleared, variables kept")

        # Now compile and save
        print("")
        print("=== Compiling and Saving ===")
        save_result = unreal.SLFAutomationLibrary.compile_and_save(vendor_bp)
        print("Save result: " + str(save_result))
    else:
        print("FAILED to clear event graphs")

    print("")
    print("=" * 60)
    print("COMPLETE")
    print("=" * 60)

# Run
clear_vendor()
