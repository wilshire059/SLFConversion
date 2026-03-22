#!/usr/bin/env python
"""
Reparent PDA_Vendor Blueprint to C++ UPDA_Vendor
"""

import unreal

def reparent_vendor():
    unreal.log_warning("=" * 60)
    unreal.log_warning("REPARENT PDA_VENDOR TO C++")
    unreal.log_warning("=" * 60)

    # Load the vendor data asset Blueprint
    vendor_bp_path = "/Game/SoulslikeFramework/Data/Vendor/PDA_Vendor"
    vendor_bp = unreal.load_asset(vendor_bp_path)

    if not vendor_bp:
        unreal.log_warning("ERROR: Could not load PDA_Vendor Blueprint")
        return

    unreal.log_warning("Loaded: " + vendor_bp.get_name())

    # Check current parent
    current_parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(vendor_bp)
    unreal.log_warning("Current parent: " + current_parent)

    # Reparent to C++ class
    new_parent = "/Script/SLFConversion.PDA_Vendor"
    unreal.log_warning("Reparenting to: " + new_parent)

    result = unreal.SLFAutomationLibrary.reparent_blueprint(vendor_bp, new_parent)
    unreal.log_warning("Reparent result: " + str(result))

    if result:
        # Clear event graphs (keep variables)
        clear_result = unreal.SLFAutomationLibrary.clear_graphs_keep_variables(vendor_bp)
        unreal.log_warning("Clear graphs result: " + str(clear_result))

        # Compile and save
        save_result = unreal.SLFAutomationLibrary.compile_and_save(vendor_bp)
        unreal.log_warning("Save result: " + str(save_result))

        # Verify new parent
        new_parent_check = unreal.SLFAutomationLibrary.get_blueprint_parent_class(vendor_bp)
        unreal.log_warning("New parent: " + new_parent_check)
    else:
        unreal.log_warning("FAILED to reparent!")

    unreal.log_warning("=" * 60)
    unreal.log_warning("COMPLETE")
    unreal.log_warning("=" * 60)

# Run
reparent_vendor()
