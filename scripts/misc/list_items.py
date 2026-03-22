import unreal
items_path = "/Game/SoulslikeFramework/Data/Items"
assets = unreal.EditorAssetLibrary.list_assets(items_path, recursive=True, include_folder=False)
for a in assets[:5]:
    unreal.log_warning(a)
