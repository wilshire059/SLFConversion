"""Reparent W_TargetExecutionIndicator to C++ class."""
import unreal

def reparent_execution_widget():
    """Reparent W_TargetExecutionIndicator to UW_TargetExecutionIndicator."""

    widget_path = '/Game/SoulslikeFramework/Widgets/HUD/W_TargetExecutionIndicator'

    # Load the widget Blueprint
    widget_bp = unreal.load_asset(widget_path)
    if not widget_bp:
        unreal.log_error("Widget BP not found: " + widget_path)
        return False

    unreal.log("Widget BP loaded: " + widget_path)

    # Load the C++ parent class
    cpp_class_path = '/Script/SLFConversion.W_TargetExecutionIndicator'
    cpp_class = unreal.load_class(None, cpp_class_path)
    if not cpp_class:
        unreal.log_error("C++ class not found: " + cpp_class_path)
        return False

    unreal.log("C++ class loaded: " + str(cpp_class.get_name()))

    # Reparent the Blueprint
    try:
        result = unreal.BlueprintEditorLibrary.reparent_blueprint(widget_bp, cpp_class)
        if result:
            unreal.log("SUCCESS: Reparented W_TargetExecutionIndicator to C++ class")
        else:
            unreal.log_error("FAILED: Reparent returned False")
            return False
    except Exception as e:
        unreal.log_error("FAILED: " + str(e))
        return False

    # Save the asset
    unreal.EditorAssetLibrary.save_asset(widget_path)
    unreal.log("Saved: " + widget_path)

    return True

if __name__ == "__main__":
    success = reparent_execution_widget()
    if success:
        unreal.log("=== Reparenting complete ===")
    else:
        unreal.log_error("=== Reparenting FAILED ===")
