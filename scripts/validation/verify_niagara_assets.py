"""Verify Niagara assets are assigned in B_RestingPoint"""
import unreal

# Load B_RestingPoint and spawn to check component state
bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_RestingPoint"
bp = unreal.EditorAssetLibrary.load_asset(bp_path)

if bp:
    gen_class = bp.generated_class()
    if gen_class:
        cdo = unreal.get_default_object(gen_class)
        
        # Get all components
        all_comps = cdo.get_components_by_class(unreal.NiagaraComponent)
        unreal.log_warning(f"=== Niagara Components ({len(all_comps)}) ===")
        
        for comp in all_comps:
            name = comp.get_name()
            unreal.log_warning(f"Component: {name}")
            
            # Get the asset (NiagaraSystem)
            try:
                asset = comp.get_editor_property("asset")
                if asset:
                    unreal.log_warning(f"  Asset: {asset.get_path_name()}")
                else:
                    unreal.log_warning(f"  Asset: None")
            except Exception as e:
                unreal.log_warning(f"  Asset error: {e}")
            
            # Get auto_activate
            try:
                auto = comp.get_editor_property("auto_activate")
                unreal.log_warning(f"  AutoActivate: {auto}")
            except:
                pass
                
        # Also check other components
        unreal.log_warning(f"=== Other Components ===")
        all_scene = cdo.get_components_by_class(unreal.SceneComponent)
        for comp in all_scene:
            if not isinstance(comp, unreal.NiagaraComponent):
                unreal.log_warning(f"  {comp.get_class().get_name()}: {comp.get_name()}")
