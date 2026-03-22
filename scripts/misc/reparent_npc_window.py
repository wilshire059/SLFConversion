#!/usr/bin/env python
"""
Reparent W_NPC_Window widget Blueprint to C++ UW_NPC_Window class
Uses C++ SLFAutomationLibrary functions
"""

import unreal

def reparent_widget():
    unreal.log_warning("=" * 60)
    unreal.log_warning("REPARENT W_NPC_Window TO C++")
    unreal.log_warning("=" * 60)

    # Load the widget Blueprint
    widget_path = "/Game/SoulslikeFramework/Widgets/Vendor/W_NPC_Window"
    widget_bp = unreal.load_asset(widget_path)

    if not widget_bp:
        unreal.log_warning("ERROR: Could not load W_NPC_Window")
        return

    unreal.log_warning("Loaded: " + widget_bp.get_name())

    # Check current parent
    current_parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(widget_bp)
    unreal.log_warning("Current parent: " + current_parent)

    # Reparent to C++ class
    new_parent = "/Script/SLFConversion.W_NPC_Window"
    unreal.log_warning("Reparenting to: " + new_parent)

    result = unreal.SLFAutomationLibrary.reparent_blueprint(widget_bp, new_parent)
    unreal.log_warning("Reparent result: " + str(result))

    if result:
        # Clear graphs but keep variables
        clear_result = unreal.SLFAutomationLibrary.clear_graphs_keep_variables(widget_bp)
        unreal.log_warning("Clear graphs result: " + str(clear_result))

        # Compile and save
        save_result = unreal.SLFAutomationLibrary.compile_and_save(widget_bp)
        unreal.log_warning("Save result: " + str(save_result))

        # Verify new parent
        new_parent_check = unreal.SLFAutomationLibrary.get_blueprint_parent_class(widget_bp)
        unreal.log_warning("New parent: " + new_parent_check)
    else:
        unreal.log_warning("FAILED to reparent!")

    unreal.log_warning("=" * 60)
    unreal.log_warning("COMPLETE")
    unreal.log_warning("=" * 60)

# Run
reparent_widget()
