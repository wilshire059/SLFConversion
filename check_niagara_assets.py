"""Check Niagara assets in B_RestingPoint by spawning instance"""
import unreal

bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_RestingPoint"
bp = unreal.EditorAssetLibrary.load_asset(bp_path)

if not bp:
    unreal.log_warning("ERROR: Could not load B_RestingPoint")
else:
    unreal.log_warning("=== B_RestingPoint Niagara Assets ===")
    unreal.log_warning(f"Parent: {unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)}")
    
    # Get the generated class and spawn a temporary actor
    gen_class = bp.generated_class()
    if gen_class:
        # Get the CDO (default object)
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            unreal.log_warning(f"CDO class: {cdo.get_class().get_name()}")
            
            # Try to get InteractionCollision from C++ component
            try:
                ic = cdo.get_editor_property("interaction_collision")
                unreal.log_warning(f"InteractionCollision: {ic.get_name() if ic else 'None'}")
            except:
                unreal.log_warning("InteractionCollision: not found")
            
            # Try to get DefaultSceneRoot from C++ (inherited from SLFInteractableBase)
            try:
                root = cdo.get_editor_property("default_scene_root")
                unreal.log_warning(f"DefaultSceneRoot: {root.get_name() if root else 'None'}")
            except:
                unreal.log_warning("DefaultSceneRoot: not found")
                
    # Also check the Blueprint's inherited components
    scs_list = unreal.SLFAutomationLibrary.get_blueprint_scs_components(bp)
    unreal.log_warning(f"SCS Components ({len(scs_list)}):")
    for comp_info in scs_list:
        unreal.log_warning(f"  {comp_info}")
