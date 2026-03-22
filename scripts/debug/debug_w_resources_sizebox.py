import unreal

# Debug W_Resources SizeBox dimensions - compare bp_only vs migrated

def check_sizebox_properties(bp_path, project_name):
    """Check SizeBox width/height properties"""
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_error(f"ERROR: Could not load {bp_path}")
        return

    unreal.log_warning(f"=== {project_name}: W_Resources SizeBox Debug ===")

    # Get widget tree from blueprint
    widget_tree = bp.get_editor_property('widget_tree')
    if not widget_tree:
        unreal.log_error("Could not get widget_tree")
        return

    root_widget = widget_tree.get_editor_property('root_widget')
    if not root_widget:
        unreal.log_error("Could not get root_widget")
        return

    unreal.log_warning(f"Root widget: {root_widget.get_name()} ({root_widget.get_class().get_name()})")

    # Find all SizeBox widgets
    sizeboxes_to_find = ['HealthbarSizer', 'FocusbarSizer', 'StaminabarSizer']

    def find_widget_recursive(widget, name):
        if widget.get_name() == name:
            return widget

        # Check if it's a panel widget with children
        try:
            if hasattr(widget, 'get_all_children'):
                children = widget.get_all_children()
                for child in children:
                    result = find_widget_recursive(child, name)
                    if result:
                        return result
        except:
            pass

        return None

    # Try to find widgets by walking the tree
    def walk_widget_tree(widget, depth=0):
        indent = "  " * depth
        name = widget.get_name()
        classname = widget.get_class().get_name()

        if name in sizeboxes_to_find:
            unreal.log_warning(f"{indent}FOUND: {name} ({classname})")

            # Get SizeBox properties
            try:
                width_override = widget.get_editor_property('width_override')
                unreal.log_warning(f"{indent}  WidthOverride: {width_override}")
            except Exception as e:
                unreal.log_warning(f"{indent}  WidthOverride: ERROR - {e}")

            try:
                height_override = widget.get_editor_property('height_override')
                unreal.log_warning(f"{indent}  HeightOverride: {height_override}")
            except Exception as e:
                unreal.log_warning(f"{indent}  HeightOverride: ERROR - {e}")

            try:
                min_desired_width = widget.get_editor_property('min_desired_width')
                unreal.log_warning(f"{indent}  MinDesiredWidth: {min_desired_width}")
            except Exception as e:
                pass

            try:
                min_desired_height = widget.get_editor_property('min_desired_height')
                unreal.log_warning(f"{indent}  MinDesiredHeight: {min_desired_height}")
            except Exception as e:
                pass

        # Walk children
        try:
            if hasattr(widget, 'get_all_children'):
                children = widget.get_all_children()
                for child in children:
                    walk_widget_tree(child, depth + 1)
        except:
            pass

    walk_widget_tree(root_widget)

    # Also check ProgressBar properties
    progressbars_to_find = ['HealthBar_Front', 'HealthBar_Back',
                            'Focusbar_Front', 'Focusbar_Back',
                            'Staminabar_Front', 'Staminabar_Back']

    def walk_for_progressbars(widget, depth=0):
        indent = "  " * depth
        name = widget.get_name()
        classname = widget.get_class().get_name()

        if name in progressbars_to_find:
            unreal.log_warning(f"{indent}FOUND: {name} ({classname})")

            # Get ProgressBar style properties
            try:
                style = widget.get_editor_property('widget_style')
                if style:
                    unreal.log_warning(f"{indent}  WidgetStyle exists")
            except:
                pass

        # Walk children
        try:
            if hasattr(widget, 'get_all_children'):
                children = widget.get_all_children()
                for child in children:
                    walk_for_progressbars(child, depth + 1)
        except:
            pass

    walk_for_progressbars(root_widget)

    unreal.log_warning(f"=== Done: {project_name} ===")

# Run for current project (SLFConversion)
check_sizebox_properties("/Game/SoulslikeFramework/Widgets/HUD/W_Resources", "SLFConversion")
