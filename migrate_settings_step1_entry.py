"""
Step 1: Clear and reparent W_Settings_Entry ONLY.
This must be run in a SEPARATE session before processing W_Settings.
"""
import unreal

BP_PATH = "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_Entry"
CPP_CLASS = "/Script/SLFConversion.W_Settings_Entry"

unreal.log_warning("=== STEP 1: Migrating W_Settings_Entry ===")

# Load the Blueprint
bp = unreal.load_asset(BP_PATH)
if not bp:
    unreal.log_warning(f"Failed to load {BP_PATH}")
else:
    # Clear ALL Blueprint logic (EventGraph AND FunctionGraphs)
    # This is critical - ClearAllEventGraphNodes only clears EventGraph,
    # but FunctionGraphs contain function definitions that conflict with C++
    success = unreal.SLFAutomationLibrary.clear_all_blueprint_logic(bp)
    unreal.log_warning(f"ClearAllBlueprintLogic: {'SUCCESS' if success else 'FAILED'}")

    # Also clear widget delegate bindings for WidgetBlueprints
    if hasattr(unreal.SLFAutomationLibrary, 'clear_widget_delegate_bindings'):
        cleared = unreal.SLFAutomationLibrary.clear_widget_delegate_bindings(bp)
        unreal.log_warning(f"Cleared {cleared} widget bindings")

    # Reparent to C++
    success = unreal.SLFAutomationLibrary.reparent_blueprint(bp, CPP_CLASS)
    unreal.log_warning(f"Reparent to {CPP_CLASS}: {'SUCCESS' if success else 'FAILED'}")

    # Save
    unreal.EditorAssetLibrary.save_asset(BP_PATH, False)
    unreal.log_warning(f"Saved {BP_PATH}")

unreal.log_warning("=== STEP 1 COMPLETE ===")
