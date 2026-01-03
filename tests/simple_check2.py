import unreal

try:
    # Check B_Soulslike_Character
    char_path = "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character"
    char_bp = unreal.EditorAssetLibrary.load_asset(char_path)
    
    if char_bp:
        unreal.log_warning(f"=== B_Soulslike_Character ===")
        
        # Check parent via blueprint property
        if hasattr(char_bp, 'parent_class') and char_bp.parent_class:
            unreal.log_warning(f"Parent: {char_bp.parent_class.get_name()}")
        
        gen_class = char_bp.generated_class()
        if gen_class:
            # Get CDO
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                unreal.log_warning(f"CDO: {cdo.get_name()}")
                if hasattr(cdo, 'mesh'):
                    main_mesh = cdo.mesh
                    if main_mesh:
                        try:
                            mesh_asset = main_mesh.get_skinned_asset() if hasattr(main_mesh, 'get_skinned_asset') else None
                            unreal.log_warning(f"SkeletalMesh: {mesh_asset.get_name() if mesh_asset else 'NONE'}")
                        except Exception as e:
                            unreal.log_warning(f"Mesh error: {e}")
                    else:
                        unreal.log_warning("Main mesh component is None")
    else:
        unreal.log_warning("FAILED to load B_Soulslike_Character")
        
except Exception as e:
    unreal.log_error(f"Script failed: {e}")
