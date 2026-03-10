import unreal
unreal.log_warning("[CHECK] Starting ABP parent check")
abp = unreal.EditorAssetLibrary.load_asset("/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeBossNew")
if abp:
    unreal.log_warning(f"[CHECK] Loaded ABP: {abp.get_name()}")
    try:
        parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(abp)
        unreal.log_warning(f"[CHECK] ABP_SoulslikeBossNew Parent: {parent}")
    except Exception as e:
        unreal.log_warning(f"[CHECK] Error getting parent: {e}")
else:
    unreal.log_warning("[CHECK] Could not load ABP_SoulslikeBossNew")
