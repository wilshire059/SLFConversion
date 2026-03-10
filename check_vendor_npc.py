#!/usr/bin/env python
"""
Check Vendor NPC - Verify VendorAsset is set on AIInteractionManager component
"""

import unreal

# Output file for results
output_lines = []

def log(msg):
    unreal.log(msg)
    output_lines.append(msg)

def check_vendor_npc():
    log("=" * 60)
    log("CHECK VENDOR NPC - Verifying VendorAsset")
    log("=" * 60)

    # Load the vendor NPC Blueprint
    vendor_bp_path = "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseVendor"
    vendor_bp = unreal.load_asset(vendor_bp_path)

    if not vendor_bp:
        log("ERROR: Could not load vendor NPC Blueprint")
        return

    log("Loaded: " + vendor_bp.get_name())

    # Check parent class
    gen_class = vendor_bp.generated_class()
    if gen_class:
        log("Generated class: " + gen_class.get_name())

    # Search SCS for AIInteractionManager and check VendorAsset
    log("")
    log("=== Checking SCS Components ===")

    found_aim = False

    try:
        scs = vendor_bp.simple_construction_script
        if scs:
            nodes = scs.get_all_nodes()
            log("Found " + str(len(nodes)) + " SCS nodes")

            for node in nodes:
                template = node.component_template
                if template:
                    class_name = template.get_class().get_name()
                    comp_name = template.get_name()
                    log("  " + comp_name + " (" + class_name + ")")

                    if "AIInteractionManager" in class_name or "AI_InteractionManager" in comp_name or "InteractionManager" in comp_name:
                        found_aim = True
                        log("    -> This is the AIInteractionManager!")

                        # Check VendorAsset
                        try:
                            vendor_asset = template.get_editor_property("vendor_asset")
                            if vendor_asset:
                                log("    -> VendorAsset: " + vendor_asset.get_name())
                            else:
                                log("    -> VendorAsset: NULL (not set!)")
                        except Exception as e:
                            log("    -> Could not read vendor_asset: " + str(e))

                        # Check DialogAsset
                        try:
                            dialog_asset = template.get_editor_property("dialog_asset")
                            if dialog_asset:
                                log("    -> DialogAsset: " + dialog_asset.get_name())
                            else:
                                log("    -> DialogAsset: NULL")
                        except Exception as e:
                            log("    -> Could not read dialog_asset: " + str(e))

                        # Check Name
                        try:
                            name = template.get_editor_property("name")
                            log("    -> Name: " + str(name))
                        except Exception as e:
                            log("    -> Could not read name: " + str(e))
        else:
            log("No SCS found")
    except Exception as e:
        log("SCS access error: " + str(e))

    if not found_aim:
        log("WARNING: AIInteractionManager component NOT FOUND in SCS!")

    # Also check CDO
    log("")
    log("=== Checking CDO ===")

    try:
        if gen_class:
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                log("CDO: " + cdo.get_name())

                # Try to get AIInteractionManager from CDO
                try:
                    aim = cdo.get_editor_property("ai_interaction_manager")
                    if aim:
                        log("CDO.AIInteractionManager: " + aim.get_name())

                        # Check VendorAsset on CDO's component
                        try:
                            vendor_asset = aim.get_editor_property("vendor_asset")
                            if vendor_asset:
                                log("  CDO VendorAsset: " + vendor_asset.get_name())
                            else:
                                log("  CDO VendorAsset: NULL")
                        except Exception as e:
                            log("  Could not read CDO vendor_asset: " + str(e))
                    else:
                        log("CDO.AIInteractionManager: NULL")
                except Exception as e:
                    log("Could not access CDO.ai_interaction_manager: " + str(e))
    except Exception as e:
        log("CDO access error: " + str(e))

    log("")
    log("=" * 60)
    log("CHECK COMPLETE")
    log("=" * 60)

    # Write results to file
    output_path = "C:/scripts/SLFConversion/migration_cache/vendor_npc_check.txt"
    with open(output_path, "w") as f:
        f.write("\n".join(output_lines))
    unreal.log("Results written to: " + output_path)

# Run
check_vendor_npc()
