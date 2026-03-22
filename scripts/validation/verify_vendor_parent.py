#!/usr/bin/env python
"""
Verify Vendor NPC parent class - writes to file
"""

import unreal

output_lines = []

def log(msg):
    unreal.log_warning(msg)  # Use log_warning so it appears in filtered output
    output_lines.append(msg)

def verify_vendor():
    vendor_bp_path = "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseVendor"
    vendor_bp = unreal.load_asset(vendor_bp_path)

    if not vendor_bp:
        log("ERROR: Could not load vendor NPC Blueprint")
        return

    log("Blueprint: " + vendor_bp.get_name())

    # Get parent class info
    log("")
    log("=== Parent Class Check ===")
    parent_class_str = unreal.SLFAutomationLibrary.get_blueprint_parent_class(vendor_bp)
    log("Parent Class: " + parent_class_str)

    # Check if it's the correct C++ class
    if "SLFNPCShowcaseVendor" in parent_class_str:
        log("CORRECT: Blueprint is parented to C++ SLFNPCShowcaseVendor")
    elif "SLFSoulslikeNPC" in parent_class_str:
        log("PARTIAL: Blueprint parented to SLFSoulslikeNPC (parent of SLFNPCShowcaseVendor)")
        log("May need reparent to /Script/SLFConversion.SLFNPCShowcaseVendor")
    elif "B_Soulslike_NPC" in parent_class_str:
        log("WRONG: Blueprint still parented to Blueprint B_Soulslike_NPC!")
        log("Need to reparent to /Script/SLFConversion.SLFNPCShowcaseVendor")
    else:
        log("UNKNOWN parent: " + parent_class_str)

    # Get SCS components
    log("")
    log("=== SCS Components ===")
    components = unreal.SLFAutomationLibrary.get_blueprint_scs_components(vendor_bp)
    if len(components) == 0:
        log("  (no SCS components)")
    for comp in components:
        log("  " + comp)

    # Check for AIInteractionManager specifically
    has_aim = False
    for comp in components:
        if "InteractionManager" in comp:
            has_aim = True
            log("FOUND AIInteractionManager in SCS: " + comp)

    if not has_aim:
        log("")
        log("NOTE: AIInteractionManager is NOT in Blueprint SCS")
        log("It is created by C++ CreateDefaultSubobject in parent class")

    # Write to file
    output_path = "C:/scripts/SLFConversion/migration_cache/vendor_parent_check.txt"
    with open(output_path, "w") as f:
        f.write("\n".join(output_lines))

# Run
verify_vendor()
