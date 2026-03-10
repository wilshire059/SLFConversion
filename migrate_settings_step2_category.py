"""
Step 2: Clear and reparent W_Settings_CategoryEntry ONLY.
"""
import unreal

BP_PATH = "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_CategoryEntry"
CPP_CLASS = "/Script/SLFConversion.W_Settings_CategoryEntry"

unreal.log_warning("=== STEP 2: Migrating W_Settings_CategoryEntry ===")

bp = unreal.load_asset(BP_PATH)
if not bp:
    unreal.log_warning(f"Failed to load {BP_PATH}")
else:
    success = unreal.SLFAutomationLibrary.clear_all_blueprint_logic(bp)
    unreal.log_warning(f"ClearAllBlueprintLogic: {'SUCCESS' if success else 'FAILED'}")

    if hasattr(unreal.SLFAutomationLibrary, 'clear_widget_delegate_bindings'):
        cleared = unreal.SLFAutomationLibrary.clear_widget_delegate_bindings(bp)
        unreal.log_warning(f"Cleared {cleared} widget bindings")

    success = unreal.SLFAutomationLibrary.reparent_blueprint(bp, CPP_CLASS)
    unreal.log_warning(f"Reparent to {CPP_CLASS}: {'SUCCESS' if success else 'FAILED'}")

    unreal.EditorAssetLibrary.save_asset(BP_PATH, False)
    unreal.log_warning(f"Saved {BP_PATH}")

unreal.log_warning("=== STEP 2 COMPLETE ===")
