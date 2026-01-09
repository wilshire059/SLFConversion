import unreal

asset_path = "/Game/SoulslikeFramework/Data/PDA_DefaultMeshData"
asset = unreal.EditorAssetLibrary.load_asset(asset_path)

if not asset:
    unreal.log_warning("Could not load asset")
else:
    unreal.log_warning(f"Asset class: {asset.get_class().get_name()}")
    
    # List all properties
    for prop in dir(asset):
        if not prop.startswith('_'):
            try:
                val = getattr(asset, prop)
                if not callable(val):
                    unreal.log_warning(f"  {prop}: {type(val).__name__}")
            except:
                pass
    
    # Try to get MeshData
    try:
        mesh_data = asset.get_editor_property('MeshData')
        unreal.log_warning(f"\nMeshData type: {type(mesh_data)}")
        unreal.log_warning(f"MeshData has export_text: {hasattr(mesh_data, 'export_text')}")
        if hasattr(mesh_data, 'export_text'):
            text = mesh_data.export_text()
            unreal.log_warning(f"MeshData export_text ({len(text)} chars):")
            unreal.log_warning(text)
    except Exception as e:
        unreal.log_warning(f"MeshData error: {e}")
    
    # Try direct mesh properties (if on C++ class)
    for prop in ['head_mesh', 'upper_body_mesh', 'arms_mesh', 'lower_body_mesh',
                 'HeadMesh', 'UpperBodyMesh', 'ArmsMesh', 'LowerBodyMesh']:
        try:
            val = asset.get_editor_property(prop)
            unreal.log_warning(f"  {prop}: {val}")
        except:
            pass
