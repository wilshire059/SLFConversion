import unreal

asset_path = "/Game/SoulslikeFramework/Data/Items/DA_Apple"
asset = unreal.EditorAssetLibrary.load_asset(asset_path)

if asset:
    unreal.log_warning("Asset: " + asset.get_name())
    unreal.log_warning("Class: " + asset.get_class().get_name())
    unreal.log_warning("Class Path: " + asset.get_class().get_path_name())
    
    # Try to access item_information
    for prop_name in ['item_information', 'ItemInformation']:
        try:
            val = asset.get_editor_property(prop_name)
            unreal.log_warning("  " + prop_name + ": " + str(val))
        except Exception as e:
            unreal.log_warning("  " + prop_name + ": ERROR - " + str(e))
    
    # Try world_niagara_system directly
    try:
        val = asset.get_editor_property('world_niagara_system')
        unreal.log_warning("  world_niagara_system: " + str(val))
    except Exception as e:
        unreal.log_warning("  world_niagara_system: ERROR - " + str(e))
else:
    unreal.log_warning("Could not load asset")
