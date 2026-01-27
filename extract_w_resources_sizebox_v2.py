import unreal

# Extract W_Resources SizeBox dimensions from bp_only
# This will show us what the original height/width values are

bp_path = "/Game/SoulslikeFramework/Widgets/HUD/W_Resources"

bp = unreal.load_asset(bp_path)
if not bp:
    unreal.log_error("ERROR: Could not load " + bp_path)
else:
    unreal.log("Loaded: " + bp.get_name())

    # Try to get the WidgetTree from the widget blueprint directly
    try:
        # UWidgetBlueprint has a WidgetTree property
        widget_tree = bp.get_editor_property('widget_tree')
        if widget_tree:
            unreal.log("Got WidgetTree from Blueprint")

            # Get root widget
            root_widget = widget_tree.get_editor_property('root_widget')
            if root_widget:
                unreal.log("Root widget: " + root_widget.get_name() + " (" + root_widget.get_class().get_name() + ")")

            # Search for SizeBox widgets
            all_widgets = widget_tree.get_editor_property('all_widgets')
            if all_widgets:
                unreal.log("Total widgets: " + str(len(all_widgets)))
                for widget in all_widgets:
                    class_name = widget.get_class().get_name()
                    widget_name = widget.get_name()

                    if 'SizeBox' in class_name:
                        unreal.log("=== Found SizeBox: " + widget_name + " ===")

                        # Try to get size properties
                        try:
                            width_override = widget.get_editor_property('width_override')
                            height_override = widget.get_editor_property('height_override')
                            unreal.log("  Width Override: " + str(width_override))
                            unreal.log("  Height Override: " + str(height_override))
                        except Exception as e:
                            unreal.log("  Error getting size: " + str(e))

                    elif 'ProgressBar' in class_name:
                        unreal.log("=== Found ProgressBar: " + widget_name + " ===")
        else:
            unreal.log_warning("WidgetTree is None")
    except Exception as e:
        unreal.log_error("Error accessing widget tree: " + str(e))

    unreal.log("Done!")
