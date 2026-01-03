import unreal

try:
    # Check B_Soulslike_Character
    char_path = "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character"
    char_bp = unreal.EditorAssetLibrary.load_asset(char_path)
    
    if char_bp:
        unreal.log_warning(f"=== B_Soulslike_Character ===")
        
        gen_class = char_bp.generated_class()
        if gen_class:
            parent = gen_class.get_super_class()
            unreal.log_warning(f"Parent: {parent.get_name() if parent else 'None'}")
        
        # Check CDO mesh
        cdo = unreal.get_default_object(gen_class)
        if cdo and hasattr(cdo, 'mesh'):
            main_mesh = cdo.mesh
            if main_mesh:
                try:
                    mesh_asset = main_mesh.get_skinned_asset() if hasattr(main_mesh, 'get_skinned_asset') else None
                    unreal.log_warning(f"CDO SkeletalMesh: {mesh_asset.get_name() if mesh_asset else 'NONE'}")
                except Exception as e:
                    unreal.log_warning(f"CDO mesh check error: {e}")
    else:
        unreal.log_warning("FAILED to load B_Soulslike_Character")
        
    # Check B_BaseCharacter
    base_path = "/Game/SoulslikeFramework/Blueprints/_Characters/B_BaseCharacter"
    base_bp = unreal.EditorAssetLibrary.load_asset(base_path)
    
    if base_bp:
        unreal.log_warning("=== B_BaseCharacter ===")
        
        gen_class = base_bp.generated_class()
        if gen_class:
            parent = gen_class.get_super_class()
            unreal.log_warning(f"Parent: {parent.get_name() if parent else 'None'}")
    else:
        unreal.log_warning("FAILED to load B_BaseCharacter")
        
except Exception as e:
    unreal.log_error(f"Script failed: {e}")
