# debug_item_props.py
# Debug script to check available properties on item assets after reparenting

import unreal

def debug_item_properties():
    unreal.log_warning("=" * 60)
    unreal.log_warning("DEBUGGING ITEM ASSET PROPERTIES")
    unreal.log_warning("=" * 60)

    asset_path = "/Game/SoulslikeFramework/Data/Items/DA_Apple"
    asset = unreal.EditorAssetLibrary.load_asset(asset_path)

    if not asset:
        unreal.log_warning("Could not load DA_Apple")
        return

    unreal.log_warning(f"Asset: {asset.get_name()}")
    unreal.log_warning(f"Class: {asset.get_class().get_name()}")
    unreal.log_warning(f"Class Path: {asset.get_class().get_path_name()}")

    # List all properties
    unreal.log_warning("")
    unreal.log_warning("Trying to access properties:")

    # Try item_information (snake_case)
    try:
        item_info = asset.get_editor_property('item_information')
        unreal.log_warning(f"  item_information: {type(item_info)} = {item_info}")
        if item_info:
            unreal.log_warning(f"    has export_text: {hasattr(item_info, 'export_text')}")
            if hasattr(item_info, 'export_text'):
                unreal.log_warning(f"    export_text(): {item_info.export_text()[:500]}")
    except Exception as e:
        unreal.log_warning(f"  item_information: ERROR - {e}")

    # Try ItemInformation (PascalCase)
    try:
        item_info = asset.get_editor_property('ItemInformation')
        unreal.log_warning(f"  ItemInformation: {type(item_info)} = {item_info}")
    except Exception as e:
        unreal.log_warning(f"  ItemInformation: ERROR - {e}")

    # Try world_niagara_system
    try:
        niagara = asset.get_editor_property('world_niagara_system')
        unreal.log_warning(f"  world_niagara_system: {niagara}")
    except Exception as e:
        unreal.log_warning(f"  world_niagara_system: ERROR - {e}")

    # Try WorldNiagaraSystem
    try:
        niagara = asset.get_editor_property('WorldNiagaraSystem')
        unreal.log_warning(f"  WorldNiagaraSystem: {niagara}")
    except Exception as e:
        unreal.log_warning(f"  WorldNiagaraSystem: ERROR - {e}")


if __name__ == "__main__":
    debug_item_properties()
