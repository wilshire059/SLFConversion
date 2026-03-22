#!/usr/bin/env python
"""
Fix Vendor NPC - Set VendorAsset on AIInteractionManager component
"""

import unreal

def log(msg):
    unreal.log(msg)

def fix_vendor_npc():
    log("=" * 60)
    log("FIX VENDOR NPC - Setting VendorAsset")
    log("=" * 60)

    # Load the vendor NPC Blueprint
    vendor_bp_path = "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseVendor"
    vendor_bp = unreal.load_asset(vendor_bp_path)

    if not vendor_bp:
        log("ERROR: Could not load vendor NPC Blueprint")
        return False

    log("Loaded: " + vendor_bp.get_name())

    # Load the vendor data asset
    vendor_asset_path = "/Game/SoulslikeFramework/Data/Vendor/DA_ExampleVendor"
    vendor_asset = unreal.load_asset(vendor_asset_path)

    if not vendor_asset:
        log("ERROR: Could not load vendor data asset")
        return False

    log("Vendor asset: " + vendor_asset.get_name())

    # Try to access SCS (Simple Construction Script) to find AIInteractionManager
    log("")
    log("=== Searching for AIInteractionManager in SCS ===")

    ai_interaction_mgr = None

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
                    log("  SCS Node: " + comp_name + " (" + class_name + ")")

                    if "AIInteractionManager" in class_name or "AI_InteractionManager" in comp_name:
                        ai_interaction_mgr = template
                        log("  -> FOUND AIInteractionManager!")
                        break
        else:
            log("No SCS found")
    except Exception as e:
        log("SCS access error: " + str(e))

    if not ai_interaction_mgr:
        log("Could not find in SCS, trying CDO...")

        # Try CDO approach
        gen_class = vendor_bp.generated_class()
        if gen_class:
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                log("CDO: " + cdo.get_name())

                # Try to find component via property access
                try:
                    aim = cdo.get_editor_property("ai_interaction_manager")
                    if aim:
                        log("Found AIInteractionManager via property: " + aim.get_name())
                        ai_interaction_mgr = aim
                except Exception as e:
                    log("Property access failed: " + str(e))

    if not ai_interaction_mgr:
        log("ERROR: Could not find AIInteractionManager - aborting")
        return False

    # Set the VendorAsset property
    log("")
    log("=== Setting VendorAsset ===")
    log("Component: " + ai_interaction_mgr.get_name())

    try:
        # Check current value
        try:
            current_vendor = ai_interaction_mgr.get_editor_property("vendor_asset")
            log("Current VendorAsset: " + (current_vendor.get_name() if current_vendor else "None"))
        except:
            log("Current VendorAsset: (could not read)")

        # Set new value
        ai_interaction_mgr.set_editor_property("vendor_asset", vendor_asset)
        log("Set VendorAsset to: " + vendor_asset.get_name())

        # Verify
        try:
            new_vendor = ai_interaction_mgr.get_editor_property("vendor_asset")
            log("Verified VendorAsset: " + (new_vendor.get_name() if new_vendor else "None"))
        except:
            log("Verification: (could not read)")

    except Exception as e:
        log("Error setting VendorAsset: " + str(e))
        return False

    # Save the Blueprint
    log("")
    log("=== Saving Blueprint ===")
    try:
        unreal.EditorAssetLibrary.save_asset(vendor_bp_path)
        log("Saved!")
    except Exception as e:
        log("Save error: " + str(e))

    log("")
    log("=" * 60)
    log("DONE")
    log("=" * 60)

    return True

# Run
fix_vendor_npc()
