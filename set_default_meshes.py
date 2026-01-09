import unreal

def set_default_meshes():
    unreal.log_warning("=" * 60)
    unreal.log_warning("SETTING DEFAULT MESH DATA")
    unreal.log_warning("=" * 60)
    
    asset_path = "/Game/SoulslikeFramework/Data/PDA_DefaultMeshData"
    asset = unreal.EditorAssetLibrary.load_asset(asset_path)
    
    if not asset:
        unreal.log_warning(f"Could not load {asset_path}")
        return
    
    unreal.log_warning(f"Asset class: {asset.get_class().get_name()}")
    
    # Default mesh paths (Manny body parts)
    mesh_map = {
        'head_mesh': '/Game/SoulslikeFramework/Meshes/SKM/SKM_MannyHead',
        'upper_body_mesh': '/Game/SoulslikeFramework/Meshes/SKM/SKM_MannyUpperBody',
        'arms_mesh': '/Game/SoulslikeFramework/Meshes/SKM/SKM_MannyArms',
        'lower_body_mesh': '/Game/SoulslikeFramework/Meshes/SKM/SKM_MannyLowerBody',
    }
    
    # Also try physics asset
    physics_path = '/Game/SoulslikeFramework/Demo/SKM/Mannequin_UE4/Meshes/SK_Mannequin_PhysicsAsset'
    
    modified = False
    
    for prop_name, mesh_path in mesh_map.items():
        mesh = unreal.EditorAssetLibrary.load_asset(mesh_path)
        if mesh:
            try:
                asset.set_editor_property(prop_name, mesh)
                modified = True
                unreal.log_warning(f"  [OK] {prop_name}: {mesh.get_name()}")
            except Exception as e:
                unreal.log_warning(f"  [ERROR] {prop_name}: {e}")
        else:
            unreal.log_warning(f"  [SKIP] Could not load: {mesh_path}")
    
    # Try physics asset
    physics = unreal.EditorAssetLibrary.load_asset(physics_path)
    if physics:
        try:
            asset.set_editor_property('final_physics_asset', physics)
            modified = True
            unreal.log_warning(f"  [OK] final_physics_asset: {physics.get_name()}")
        except Exception as e:
            unreal.log_warning(f"  [ERROR] final_physics_asset: {e}")
    
    if modified:
        unreal.EditorAssetLibrary.save_asset(asset_path, only_if_is_dirty=False)
        unreal.log_warning(f"\nSaved {asset_path}")
    else:
        unreal.log_warning("\nNo changes made")

if __name__ == "__main__":
    set_default_meshes()
