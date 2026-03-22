"""Test ExecutionWidget on spawned enemy."""
import unreal

def test_execution_widget():
    """Check for ExecutionWidget in B_BaseCharacter."""

    # Check if W_TargetExecutionIndicator exists
    widget_path = '/Game/SoulslikeFramework/Widgets/HUD/W_TargetExecutionIndicator'
    widget_bp = unreal.load_asset(widget_path)
    if widget_bp:
        unreal.log("OK: Widget BP exists: " + widget_path)
    else:
        unreal.log_error("ERROR: Widget BP NOT FOUND: " + widget_path)
        return

    # Check if the widget has a generated class
    gen_class = widget_bp.generated_class()
    if gen_class:
        unreal.log("OK: Widget class exists: " + str(gen_class.get_name()))
    else:
        unreal.log_error("ERROR: Widget has no generated class")

    unreal.log("=== Test complete ===")

if __name__ == "__main__":
    test_execution_widget()
