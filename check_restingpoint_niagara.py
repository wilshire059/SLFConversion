"""Check B_RestingPoint Niagara component assets"""
import unreal

bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_RestingPoint"
bp = unreal.EditorAssetLibrary.load_asset(bp_path)

if not bp:
    unreal.log_warning("ERROR: Could not load B_RestingPoint")
else:
    unreal.log_warning(f"=== B_RestingPoint Niagara Check ===")
    unreal.log_warning(f"Parent Class: {unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)}")
    
    # Check the SCS for Niagara components and their templates
    scs = bp.get_editor_property("simple_construction_script")
    if scs:
        all_nodes = scs.get_all_nodes()
        unreal.log_warning(f"SCS Node Count: {len(all_nodes)}")
        
        for node in all_nodes:
            if node.component_template:
                comp = node.component_template
                comp_name = comp.get_name()
                comp_class = comp.get_class().get_name()
                
                if "Niagara" in comp_class:
                    unreal.log_warning(f"  NiagaraComp: {comp_name}")
                    try:
                        asset = comp.get_editor_property("asset")
                        if asset:
                            unreal.log_warning(f"    Asset: {asset.get_path_name()}")
                        else:
                            unreal.log_warning(f"    Asset: None")
                        auto_activate = comp.get_editor_property("auto_activate")
                        unreal.log_warning(f"    AutoActivate: {auto_activate}")
                    except Exception as e:
                        unreal.log_warning(f"    Error: {e}")
