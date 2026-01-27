"""Check DA_Greatsword original overlay data."""
import unreal

asset = unreal.EditorAssetLibrary.load_asset("/Game/SoulslikeFramework/Data/Items/DA_Greatsword")
if asset:
    item_info = asset.get_editor_property('ItemInformation')
    if item_info:
        export = item_info.export_text()
        unreal.log_warning("=== DA_Greatsword ItemInformation export ===")
        # Print all lines - look for Overlay data
        for line in export.split(','):
            line = line.strip()
            if 'Overlay' in line or 'Weapon' in line or 'Equipment' in line:
                unreal.log_warning(f"  {line}")

        # Also print the full export for analysis
        unreal.log_warning("=== Full export (first 2000 chars) ===")
        unreal.log_warning(export[:2000])
