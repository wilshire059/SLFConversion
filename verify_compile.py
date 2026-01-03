import unreal

def run():
    unreal.log_warning("=== VERIFY COMPILE: Compiling all Blueprints ===")
    
    # Get all Blueprint assets
    asset_registry = unreal.AssetRegistryHelpers.get_asset_registry()
    
    # Try to compile the AnimBPs specifically
    animbp_paths = [
        "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeBossNew",
        "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeEnemy",
        "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeNPC",
    ]
    
    for path in animbp_paths:
        bp = unreal.EditorAssetLibrary.load_asset(path)
        if bp:
            unreal.log_warning(f"Compiling: {path}")
            # Force recompile
            unreal.KismetEditorUtilities.compile_blueprint(bp)
            status = bp.get_editor_property("status")
            unreal.log_warning(f"  Status: {status}")

run()
