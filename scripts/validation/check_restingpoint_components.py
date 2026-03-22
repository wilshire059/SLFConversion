"""Check B_RestingPoint component structure"""
import unreal

bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_RestingPoint"
bp = unreal.EditorAssetLibrary.load_asset(bp_path)

if not bp:
    unreal.log("ERROR: Could not load B_RestingPoint")
else:
    unreal.log(f"Blueprint: {bp.get_name()}")
    
    # Get parent class
    parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
    unreal.log(f"Parent Class: {parent}")
    
    # Check SCS components
    scs_comps = unreal.SLFAutomationLibrary.get_blueprint_scs_components(bp)
    unreal.log(f"SCS Components: {len(scs_comps)}")
    for comp in scs_comps:
        unreal.log(f"  - {comp}")
