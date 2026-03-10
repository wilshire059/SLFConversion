"""
Clear ALL Blueprint logic from vendor widgets that still have compile errors.
Uses ClearAllBlueprintLogic to clear both EventGraphs AND function graphs.
"""
import unreal

# Blueprints that need ALL logic cleared (not just EventGraphs)
VENDOR_PATHS = [
    "/Game/SoulslikeFramework/Data/Dialog/PDA_Dialog",
    "/Game/SoulslikeFramework/Widgets/Vendor/W_VendorAction",
    "/Game/SoulslikeFramework/Widgets/Vendor/W_VendorSlot",
    "/Game/SoulslikeFramework/Widgets/Vendor/W_NPC_Window_Vendor",
]

def clear_vendor_logic():
    """Clear ALL Blueprint logic from vendor widgets."""
    unreal.log_warning("\n=== Clearing Vendor Widget Logic ===\n")

    for bp_path in VENDOR_PATHS:
        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            unreal.log_warning(f"MISSING: {bp_path}")
            continue

        bp_name = bp.get_name()
        unreal.log_warning(f"Processing: {bp_name}")

        # Use ClearGraphsKeepVariables to preserve variables but clear all logic
        try:
            success = unreal.SLFAutomationLibrary.clear_graphs_keep_variables(bp)
            if success:
                unreal.log_warning(f"  CLEARED ALL GRAPHS: {bp_name}")
            else:
                unreal.log_warning(f"  CLEAR FAILED: {bp_name}")
        except Exception as e:
            unreal.log_warning(f"  ERROR: {bp_name} - {e}")

        # Save
        try:
            unreal.EditorAssetLibrary.save_asset(bp_path)
            unreal.log_warning(f"  SAVED: {bp_name}")
        except Exception as e:
            unreal.log_warning(f"  SAVE ERROR: {bp_name} - {e}")

if __name__ == "__main__":
    clear_vendor_logic()
    unreal.log_warning("\n=== Done ===")
