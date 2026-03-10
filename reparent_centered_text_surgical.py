"""
Surgical reparent of W_Settings_CenteredText to C++ UW_Settings_CenteredText
Using SLFAutomationLibrary.reparent_blueprint (takes string path, not UClass)
"""
import unreal

def main():
    unreal.log_warning("=" * 60)
    unreal.log_warning("SURGICAL REPARENT: W_Settings_CenteredText")
    unreal.log_warning("=" * 60)

    bp_path = "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_CenteredText"
    cpp_class_path = "/Script/SLFConversion.W_Settings_CenteredText"

    # Load blueprint
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_error(f"Failed to load Blueprint: {bp_path}")
        return

    unreal.log_warning(f"Blueprint: {bp.get_name()}")

    # Check parent_class property
    if hasattr(bp, 'parent_class') and bp.parent_class:
        unreal.log_warning(f"Current parent: {bp.parent_class.get_name()}")

    # Step 1: Clear event graphs
    unreal.log_warning("Step 1: Clearing event graphs...")
    try:
        cleared = unreal.SLFAutomationLibrary.clear_event_graphs(bp)
        unreal.log_warning(f"Cleared {cleared} nodes")
    except Exception as e:
        unreal.log_warning(f"clear_event_graphs: {e}")

    # Step 2: Save after clearing
    unreal.log_warning("Step 2: Saving after clear...")
    unreal.EditorAssetLibrary.save_asset(bp_path)

    # Step 3: Reparent using SLFAutomationLibrary (takes string path, not UClass!)
    unreal.log_warning(f"Step 3: Reparenting to {cpp_class_path}...")
    try:
        # Use SLFAutomationLibrary.reparent_blueprint - takes (BlueprintAsset, StringPath)
        result = unreal.SLFAutomationLibrary.reparent_blueprint(bp, cpp_class_path)
        unreal.log_warning(f"Reparent result: {result}")

        if result:
            unreal.log_warning("SUCCESS!")

            # Step 4: Save
            unreal.log_warning("Step 4: Saving after reparent...")
            unreal.EditorAssetLibrary.save_asset(bp_path)

            # Verify
            if hasattr(bp, 'parent_class') and bp.parent_class:
                unreal.log_warning(f"New parent: {bp.parent_class.get_name()}")
        else:
            unreal.log_error("Reparent failed!")
            if hasattr(bp, 'parent_class') and bp.parent_class:
                unreal.log_warning(f"Parent still: {bp.parent_class.get_name()}")

    except Exception as e:
        unreal.log_error(f"Exception: {e}")
        import traceback
        unreal.log_error(traceback.format_exc())

    unreal.log_warning("=" * 60)

main()
