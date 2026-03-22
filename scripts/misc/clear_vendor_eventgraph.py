#!/usr/bin/env python
"""
Clear Vendor NPC EventGraph - Remove Blueprint BeginPlay so C++ BeginPlay runs
"""

import unreal

output_lines = []

def log(msg):
    unreal.log(msg)
    output_lines.append(msg)

def clear_vendor_eventgraph():
    log("=" * 60)
    log("CLEAR VENDOR NPC EVENTGRAPH")
    log("=" * 60)

    # Load the vendor NPC Blueprint
    vendor_bp_path = "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseVendor"
    vendor_bp = unreal.EditorAssetLibrary.load_blueprint_class(vendor_bp_path)

    if not vendor_bp:
        # Try alternate loading method
        vendor_bp = unreal.load_asset(vendor_bp_path)

    if not vendor_bp:
        log("ERROR: Could not load vendor NPC Blueprint")
        return False

    log("Loaded: " + str(vendor_bp))

    # Get the Blueprint object
    bp_obj = None
    if hasattr(vendor_bp, 'generated_class'):
        # This is a UBlueprint
        bp_obj = vendor_bp
    else:
        # Try to get Blueprint from class
        log("Trying to load as class...")
        bp_asset = unreal.load_asset(vendor_bp_path)
        if bp_asset:
            bp_obj = bp_asset
            log("Loaded as asset: " + str(bp_obj))

    if not bp_obj:
        log("ERROR: Could not get Blueprint object")
        return False

    # Try to clear using BlueprintEditorLibrary
    log("")
    log("=== Attempting to clear EventGraph ===")

    try:
        # Check if we can access function graphs
        if hasattr(bp_obj, 'function_graphs'):
            log("Found function_graphs attribute")

        # Try using SLFAutomationLibrary if available
        if hasattr(unreal, 'SLFAutomationLibrary'):
            log("Using SLFAutomationLibrary.clear_event_graph...")
            result = unreal.SLFAutomationLibrary.clear_event_graph(bp_obj)
            log("Result: " + str(result))
        else:
            log("SLFAutomationLibrary not available")

            # Try to manually clear using Kismet graph manipulation
            # This requires EditorBlueprintLibrary or KismetEditorLibrary
            if hasattr(unreal, 'BlueprintEditorLibrary'):
                log("Trying BlueprintEditorLibrary...")
                # Note: There's no direct ClearEventGraph function in Python API
                # We need to use a different approach

            # Alternative: Mark all nodes in EventGraph for deletion
            log("Direct graph clearing not available via Python API")
            log("Recommendation: Use C++ SLFAutomationLibrary::ClearEventGraph()")

    except Exception as e:
        log("Error during clearing: " + str(e))

    log("")
    log("=" * 60)
    log("COMPLETE")
    log("=" * 60)

    # Write results
    output_path = "C:/scripts/SLFConversion/migration_cache/clear_vendor_eventgraph.txt"
    with open(output_path, "w") as f:
        f.write("\n".join(output_lines))
    log("Results written to: " + output_path)

    return True

# Run
clear_vendor_eventgraph()
