"""
Step 3: Clear and reparent PDA_CustomSettings ONLY.
"""
import unreal

BP_PATH = "/Game/SoulslikeFramework/Data/PDA_CustomSettings"
CPP_CLASS = "/Script/SLFConversion.PDA_CustomSettings"

unreal.log_warning("=== STEP 3: Migrating PDA_CustomSettings ===")

bp = unreal.load_asset(BP_PATH)
if not bp:
    unreal.log_warning(f"Failed to load {BP_PATH}")
else:
    success = unreal.SLFAutomationLibrary.clear_all_blueprint_logic(bp)
    unreal.log_warning(f"ClearAllBlueprintLogic: {'SUCCESS' if success else 'FAILED'}")

    success = unreal.SLFAutomationLibrary.reparent_blueprint(bp, CPP_CLASS)
    unreal.log_warning(f"Reparent to {CPP_CLASS}: {'SUCCESS' if success else 'FAILED'}")

    unreal.EditorAssetLibrary.save_asset(BP_PATH, False)
    unreal.log_warning(f"Saved {BP_PATH}")

unreal.log_warning("=== STEP 3 COMPLETE ===")
