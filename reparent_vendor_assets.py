#!/usr/bin/env python
"""
Reparent all vendor-related assets to C++
"""

import unreal

def reparent_asset(bp_path, new_parent):
    unreal.log_warning("Processing: " + bp_path)

    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_warning("  ERROR: Could not load")
        return False

    # Check current parent
    current_parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
    unreal.log_warning("  Current parent: " + current_parent)

    if new_parent in current_parent:
        unreal.log_warning("  Already reparented to C++")
        return True

    # Reparent
    result = unreal.SLFAutomationLibrary.reparent_blueprint(bp, new_parent)
    if result:
        # Clear graphs
        unreal.SLFAutomationLibrary.clear_graphs_keep_variables(bp)
        # Compile and save
        unreal.SLFAutomationLibrary.compile_and_save(bp)
        unreal.log_warning("  SUCCESS")
        return True
    else:
        unreal.log_warning("  FAILED")
        return False

def main():
    unreal.log_warning("=" * 60)
    unreal.log_warning("REPARENT VENDOR ASSETS TO C++")
    unreal.log_warning("=" * 60)

    # Reparent PDA_Vendor (parent class) if not already done
    reparent_asset(
        "/Game/SoulslikeFramework/Data/Vendor/PDA_Vendor",
        "/Script/SLFConversion.PDA_Vendor"
    )

    # Reparent DA_ExampleVendor (instance) to PDA_Vendor
    # Note: Data asset instances should use the PDA_Vendor path since that's the Blueprint class
    reparent_asset(
        "/Game/SoulslikeFramework/Data/Vendor/DA_ExampleVendor",
        "/Game/SoulslikeFramework/Data/Vendor/PDA_Vendor"
    )

    unreal.log_warning("=" * 60)
    unreal.log_warning("COMPLETE")
    unreal.log_warning("=" * 60)

main()
