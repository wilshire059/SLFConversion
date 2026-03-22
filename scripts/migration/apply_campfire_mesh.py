"""Apply SM_Campfire mesh to B_RestingPoint's DefaultInteractableMesh property"""
import unreal

# Paths
bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_RestingPoint"
mesh_path = "/Game/SoulslikeFramework/Meshes/SM/Campfire/SM_Campfire"

# Load the mesh
mesh = unreal.EditorAssetLibrary.load_asset(mesh_path)
if not mesh:
    unreal.log_warning(f"ERROR: Could not load mesh: {mesh_path}")
else:
    unreal.log_warning(f"Loaded mesh: {mesh.get_name()}")

    # Load the Blueprint
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        unreal.log_warning(f"ERROR: Could not load Blueprint: {bp_path}")
    else:
        # Get generated class and CDO
        gen_class = bp.generated_class()
        if gen_class:
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                # Set the DefaultInteractableMesh property
                try:
                    cdo.set_editor_property("default_interactable_mesh", mesh)
                    unreal.log_warning(f"Set DefaultInteractableMesh to: {mesh.get_name()}")
                    
                    # Mark blueprint as modified and save
                    unreal.EditorAssetLibrary.save_asset(bp_path, only_if_is_dirty=False)
                    unreal.log_warning(f"Saved: {bp_path}")
                except Exception as e:
                    unreal.log_warning(f"Error setting property: {e}")
            else:
                unreal.log_warning("ERROR: Could not get CDO")
        else:
            unreal.log_warning("ERROR: Could not get generated class")
