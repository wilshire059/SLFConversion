"""Verify the CDO has DefaultInteractableMesh set correctly"""
import unreal

bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_RestingPoint"
bp = unreal.EditorAssetLibrary.load_asset(bp_path)

if bp:
    gen_class = bp.generated_class()
    if gen_class:
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            unreal.log_warning(f"=== B_RestingPoint CDO Properties ===")
            
            # Check DefaultInteractableMesh
            try:
                mesh_ref = cdo.get_editor_property("default_interactable_mesh")
                if mesh_ref:
                    # Try to load it
                    unreal.log_warning(f"DefaultInteractableMesh: {mesh_ref}")
                else:
                    unreal.log_warning("DefaultInteractableMesh: None/Empty")
            except Exception as e:
                unreal.log_warning(f"Error getting DefaultInteractableMesh: {e}")
            
            # Also check if InteractableSM component exists on CDO
            try:
                sm_comp = cdo.get_editor_property("interactable_sm")
                if sm_comp:
                    mesh = sm_comp.get_editor_property("static_mesh")
                    unreal.log_warning(f"InteractableSM component mesh: {mesh.get_name() if mesh else 'None'}")
                else:
                    unreal.log_warning("InteractableSM: None")
            except Exception as e:
                unreal.log_warning(f"Error getting InteractableSM: {e}")
