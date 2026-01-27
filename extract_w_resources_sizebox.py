import unreal

# Extract W_Resources SizeBox dimensions from bp_only
# This will show us what the original height/width values are

bp_path = "/Game/SoulslikeFramework/Widgets/HUD/W_Resources"

bp = unreal.load_asset(bp_path)
if not bp:
    print(f"ERROR: Could not load {bp_path}")
else:
    print(f"Loaded: {bp.get_name()}")

    # Get the widget tree root
    gen_class = bp.generated_class()
    if gen_class:
        # Create a default instance to read widget tree
        cdo = gen_class.get_default_object()
        if cdo:
            # Try to get widget tree
            widget_tree = getattr(cdo, 'widget_tree', None)
            if widget_tree:
                print(f"Widget tree: {widget_tree}")
            else:
                print("No widget_tree attribute found")

    # Try to get the SizeBox properties from the widget blueprint directly
    # The WidgetTree is stored on the UWidgetBlueprint itself
    try:
        # UWidgetBlueprint has a WidgetTree property
        widget_tree = bp.get_editor_property('widget_tree')
        if widget_tree:
            print(f"Got WidgetTree from Blueprint")

            # Get root widget
            root_widget = widget_tree.get_editor_property('root_widget')
            if root_widget:
                print(f"Root widget: {root_widget.get_name()} ({root_widget.get_class().get_name()})")

            # Search for SizeBox widgets
            all_widgets = widget_tree.get_editor_property('all_widgets')
            if all_widgets:
                print(f"Total widgets: {len(all_widgets)}")
                for widget in all_widgets:
                    class_name = widget.get_class().get_name()
                    widget_name = widget.get_name()

                    if 'SizeBox' in class_name or 'Sizer' in widget_name:
                        print(f"  Found SizeBox: {widget_name}")

                        # Try to get size properties
                        try:
                            width_override = widget.get_editor_property('width_override')
                            height_override = widget.get_editor_property('height_override')
                            print(f"    Width Override: {width_override}")
                            print(f"    Height Override: {height_override}")
                        except Exception as e:
                            print(f"    Error getting size: {e}")

                        # Try to check if overrides are enabled
                        try:
                            b_override_width = widget.get_editor_property('b_override_width_override')
                            b_override_height = widget.get_editor_property('b_override_height_override')
                            print(f"    Override Width Enabled: {b_override_width}")
                            print(f"    Override Height Enabled: {b_override_height}")
                        except Exception as e:
                            pass  # Property might not exist

                    elif 'ProgressBar' in class_name:
                        print(f"  Found ProgressBar: {widget_name}")
                        # Check ProgressBar style
                        try:
                            style = widget.get_editor_property('widget_style')
                            if style:
                                print(f"    Has WidgetStyle")
                        except:
                            pass
    except Exception as e:
        print(f"Error accessing widget tree: {e}")

    print("Done!")
