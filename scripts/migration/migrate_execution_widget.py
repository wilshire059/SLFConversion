"""Migrate W_TargetExecutionIndicator to C++ - clear function graphs then reparent."""
import unreal

def migrate_single_widget(widget_path):
    """Clear BP function graphs and reparent a single widget to C++ class."""

    # Load the widget Blueprint
    widget_bp = unreal.load_asset(widget_path)
    if not widget_bp:
        unreal.log_error("Widget BP not found: " + widget_path)
        return False

    unreal.log("Widget BP loaded: " + widget_path)

    # Use SLFAutomationLibrary to clear all Blueprint logic (removes conflicting "Visible?" parameter)
    # This is the same pattern used in run_migration.py
    unreal.log("Clearing Blueprint logic with SLFAutomationLibrary...")
    unreal.SLFAutomationLibrary.clear_all_blueprint_logic_no_compile(widget_bp)
    unreal.log("Blueprint logic cleared")

    # Remove Blueprint interfaces - the BP implements BPI_ExecutionIndicator_C with "Visible?"
    # but C++ parent uses ISLFExecutionIndicatorInterface with "Visible"
    # Removing the BP interface allows the C++ interface to take over
    unreal.log("Removing Blueprint interfaces...")
    removed_count = unreal.SLFAutomationLibrary.remove_implemented_interfaces(widget_bp)
    unreal.log("Removed {} interfaces".format(removed_count))

    # Save after clearing logic AND removing interfaces
    unreal.EditorAssetLibrary.save_asset(widget_path)
    unreal.log("Saved widget after clearing graphs and removing interfaces")

    # Reload after save to ensure clean state
    widget_bp = unreal.load_asset(widget_path)
    if not widget_bp:
        unreal.log_error("Failed to reload widget after save!")
        return False
    unreal.log("Reloaded widget BP after save")

    # Load the C++ parent class
    cpp_class_path = '/Script/SLFConversion.W_TargetExecutionIndicator'
    cpp_class = unreal.load_class(None, cpp_class_path)
    if not cpp_class:
        unreal.log_error("C++ class not found: " + cpp_class_path)
        return False

    unreal.log("C++ class loaded: " + str(cpp_class.get_name()))

    # Log current state
    gen_class = widget_bp.generated_class()
    unreal.log("Generated class: " + str(gen_class.get_name() if gen_class else "None"))

    # Reparent the Blueprint using SLFAutomationLibrary (same as run_migration.py)
    unreal.log("Attempting reparent with SLFAutomationLibrary...")
    try:
        result = unreal.SLFAutomationLibrary.reparent_blueprint(widget_bp, cpp_class_path)
        unreal.log("reparent_blueprint returned: " + str(result))
        if result:
            unreal.log("SUCCESS: Reparented W_TargetExecutionIndicator to C++ class")
        else:
            unreal.log_error("FAILED: Reparent returned False - check LogSLFAutomation for details")
            return False
    except Exception as e:
        unreal.log_error("FAILED with exception: " + str(e))
        return False

    # Save the asset
    unreal.EditorAssetLibrary.save_asset(widget_path)
    unreal.log("Saved: " + widget_path)

    # Verify
    widget_bp = unreal.load_asset(widget_path)
    gen_class = widget_bp.generated_class()
    if gen_class:
        unreal.log("Verified - Generated class: " + str(gen_class.get_name()))

    return True

def migrate_execution_widget():
    """Migrate both HUD and World versions of W_TargetExecutionIndicator."""

    # Both paths where the widget may exist
    widget_paths = [
        '/Game/SoulslikeFramework/Widgets/HUD/W_TargetExecutionIndicator',
        '/Game/SoulslikeFramework/Widgets/World/W_TargetExecutionIndicator',
    ]

    success_count = 0
    for path in widget_paths:
        unreal.log("=" * 60)
        unreal.log("Migrating: " + path)
        unreal.log("=" * 60)
        if migrate_single_widget(path):
            success_count += 1
        else:
            unreal.log_warning("Failed or skipped: " + path)

    unreal.log("=" * 60)
    unreal.log("Migration complete: {}/{} widgets succeeded".format(success_count, len(widget_paths)))
    return success_count > 0

if __name__ == "__main__":
    success = migrate_execution_widget()
    if success:
        unreal.log("=== Migration complete ===")
    else:
        unreal.log_error("=== Migration FAILED ===")
