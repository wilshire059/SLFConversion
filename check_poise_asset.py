import unreal

def log(msg):
    print(f"[PoiseAsset] {msg}")
    unreal.log_warning(f"[PoiseAsset] {msg}")

log("=" * 70)
log("POISE BREAK ASSET CHECK")
log("=" * 70)

# Try to load the asset
asset_path = "/Game/SoulslikeFramework/Data/_AnimationData/PDA_PoiseBreakAnimData"
asset = unreal.EditorAssetLibrary.load_asset(asset_path)

if asset:
    log(f"Asset loaded: {asset}")
    log(f"Asset class: {asset.get_class().get_name()}")
    log(f"Asset class path: {asset.get_class().get_path_name()}")

    # Check properties
    try:
        stagger = asset.get_editor_property("stagger_montage")
        log(f"StaggerMontage: {stagger}")
    except Exception as e:
        log(f"StaggerMontage error: {e}")
else:
    log(f"ERROR: Could not load asset at {asset_path}")

    # List files in the directory
    log("Checking directory contents...")
    dir_path = "/Game/SoulslikeFramework/Data/_AnimationData"
    assets = unreal.EditorAssetLibrary.list_assets(dir_path, recursive=False)
    for a in assets:
        log(f"  - {a}")

log("=" * 70)
