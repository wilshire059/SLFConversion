"""Set ExecutionWidget WidgetClass using SubobjectDataSubsystem."""
import unreal

def set_execution_widget_class():
    """Set the WidgetClass on ExecutionWidget component in B_BaseCharacter."""

    # Load the widget class
    widget_path = '/Game/SoulslikeFramework/Widgets/HUD/W_TargetExecutionIndicator'
    widget_bp = unreal.load_asset(widget_path)
    if not widget_bp:
        unreal.log_error("Widget BP not found")
        return False

    widget_class = widget_bp.generated_class()
    unreal.log("Widget class: " + str(widget_class))

    # Load B_BaseCharacter
    char_path = '/Game/SoulslikeFramework/Blueprints/_Characters/B_BaseCharacter'
    char_bp = unreal.load_asset(char_path)
    if not char_bp:
        unreal.log_error("Character BP not found")
        return False

    # Get the Blueprint's generated class
    gen_class = char_bp.generated_class()

    # Use SubobjectDataSubsystem to access SCS components
    try:
        subsystem = unreal.get_engine_subsystem(unreal.SubobjectDataSubsystem)
        if subsystem:
            # Get handles for the Blueprint
            handles = subsystem.k2_gather_subobject_data_for_blueprint(char_bp)
            unreal.log("Subobject handles: " + str(len(handles)))

            for handle in handles:
                data = subsystem.k2_find_subobject_data_from_handle(handle)
                if data:
                    # Get the object from the data
                    obj = data.get_object()
                    if obj:
                        obj_name = obj.get_name()
                        if 'Execution' in obj_name:
                            unreal.log("FOUND: " + obj_name + " (" + obj.get_class().get_name() + ")")
                            # Check if it's a WidgetComponent
                            if obj.get_class().get_name() == 'WidgetComponent':
                                try:
                                    obj.set_editor_property('widget_class', widget_class)
                                    unreal.log("SUCCESS: Set widget_class on ExecutionWidget")
                                except Exception as e:
                                    unreal.log_error("Failed to set widget_class: " + str(e))
        else:
            unreal.log_warning("SubobjectDataSubsystem not available")
    except Exception as e:
        unreal.log_warning("SubobjectDataSubsystem failed: " + str(e))

    # Save the Blueprint
    try:
        unreal.EditorAssetLibrary.save_asset(char_path)
        unreal.log("Saved: " + char_path)
    except:
        pass

    return True

if __name__ == "__main__":
    set_execution_widget_class()
