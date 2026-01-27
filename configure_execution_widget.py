"""Configure ExecutionWidget in B_BaseCharacter to use W_TargetExecutionIndicator."""
import unreal

def configure_execution_widget():
    """Set the WidgetClass on ExecutionWidget component."""

    # Load the widget class
    widget_path = '/Game/SoulslikeFramework/Widgets/HUD/W_TargetExecutionIndicator'
    widget_bp = unreal.load_asset(widget_path)
    if not widget_bp:
        unreal.log_error("ERROR: Widget BP not found: " + widget_path)
        return False

    widget_class = widget_bp.generated_class()
    if not widget_class:
        unreal.log_error("ERROR: Widget has no generated class")
        return False

    unreal.log("Widget class: " + str(widget_class.get_name()))

    # Load B_BaseCharacter
    char_path = '/Game/SoulslikeFramework/Blueprints/_Characters/B_BaseCharacter'
    char_bp = unreal.load_asset(char_path)
    if not char_bp:
        unreal.log_error("ERROR: Character BP not found: " + char_path)
        return False

    unreal.log("Character BP loaded: " + char_path)

    # Get CDO and find ExecutionWidget component
    gen_class = char_bp.generated_class()
    if not gen_class:
        unreal.log_error("ERROR: Character has no generated class")
        return False

    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        unreal.log_error("ERROR: Cannot get CDO")
        return False

    # Find the ExecutionWidget component in the CDO
    # This might not work directly - SCS components need different approach
    unreal.log("CDO class: " + str(cdo.get_class().get_name()))

    # Try to access the component via Blueprint SCS
    # The ExecutionWidget is defined in Blueprint SCS, not C++
    # We need to modify the Blueprint's SCS node

    # Use BlueprintEditorLibrary to get SCS
    try:
        # Get all SimpleConstructionScript nodes
        scs = unreal.BlueprintEditorLibrary.get_simple_construction_script(char_bp)
        if scs:
            nodes = scs.get_all_nodes()
            unreal.log("SCS Nodes: " + str(len(nodes)))
            for node in nodes:
                template = node.get_component_template()
                if template:
                    comp_name = template.get_name()
                    if 'Execution' in comp_name:
                        unreal.log("FOUND: ExecutionWidget component")
                        # Try to set WidgetClass
                        try:
                            template.set_editor_property('widget_class', widget_class)
                            unreal.log("SET: widget_class to " + str(widget_class.get_name()))
                        except Exception as e:
                            unreal.log_warning("Could not set widget_class via set_editor_property: " + str(e))
        else:
            unreal.log_warning("No SCS found")
    except Exception as e:
        unreal.log_warning("BlueprintEditorLibrary approach failed: " + str(e))

    # Save the character Blueprint
    unreal.EditorAssetLibrary.save_asset(char_path)
    unreal.log("SAVED: " + char_path)

    return True

if __name__ == "__main__":
    success = configure_execution_widget()
    if success:
        unreal.log("=== Configuration complete ===")
    else:
        unreal.log_error("=== Configuration FAILED ===")
