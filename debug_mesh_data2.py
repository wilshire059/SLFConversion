import unreal

# Load as Blueprint and get its generated class
bp_path = "/Game/SoulslikeFramework/Data/PDA_DefaultMeshData"
bp = unreal.EditorAssetLibrary.load_asset(bp_path)

unreal.log_warning(f"Loaded: {bp}")
unreal.log_warning(f"Type: {type(bp)}")

if bp:
    # Try to get the generated class
    if hasattr(bp, 'generated_class'):
        gen_class = bp.generated_class()
        unreal.log_warning(f"Generated class: {gen_class}")
        
        # Get CDO
        cdo = gen_class.get_default_object()
        unreal.log_warning(f"CDO: {cdo}")
        
        if cdo:
            # Try to get MeshData from CDO
            try:
                mesh_data = cdo.get_editor_property('MeshData')
                unreal.log_warning(f"MeshData: {mesh_data}")
                if mesh_data and hasattr(mesh_data, 'export_text'):
                    unreal.log_warning(f"MeshData export: {mesh_data.export_text()}")
            except Exception as e:
                unreal.log_warning(f"MeshData error: {e}")
                
            # List all properties on CDO
            unreal.log_warning("\nCDO properties:")
            for prop_name in ['MeshData', 'mesh_data', 'FinalPhysicsAsset', 'final_physics_asset']:
                try:
                    val = cdo.get_editor_property(prop_name)
                    unreal.log_warning(f"  {prop_name}: {val}")
                except Exception as e:
                    unreal.log_warning(f"  {prop_name}: ERROR - {e}")
