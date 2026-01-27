"""Check B_Interactable SCS components"""
import unreal

# Check B_Interactable
bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Interactable"
bp = unreal.EditorAssetLibrary.load_asset(bp_path)

if not bp:
    unreal.log_warning("ERROR: Could not load B_Interactable")
else:
    unreal.log_warning("=== B_Interactable SCS Check ===")
    unreal.log_warning(f"Parent: {unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)}")
    
    scs_list = unreal.SLFAutomationLibrary.get_blueprint_scs_components(bp)
    unreal.log_warning(f"SCS Components ({len(scs_list)}):")
    for comp_info in scs_list:
        unreal.log_warning(f"  {comp_info}")
        
# Also check bp_only version for comparison
unreal.log_warning("=== Comparing with bp_only (same project, should match now) ===")
