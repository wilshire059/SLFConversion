import unreal
import os

# Extract W_Resources widget tree properties
bp_path = "/Game/SoulslikeFramework/Widgets/HUD/W_Resources"

bp = unreal.load_asset(bp_path)
if not bp:
    unreal.log_error("ERROR: Could not load " + bp_path)
else:
    unreal.log_warning("=== Extracting W_Resources Widget Tree ===")

    # Get widget tree directly
    try:
        widget_tree = bp.get_editor_property('widget_tree')
        if widget_tree:
            unreal.log_warning("WidgetTree object found")

            all_widgets = widget_tree.get_editor_property('all_widgets')
            if all_widgets:
                unreal.log_warning(f"Found {len(all_widgets)} widgets in tree")

                # Look for SizeBox widgets and log their properties
                for w in all_widgets:
                    name = w.get_name()
                    classname = w.get_class().get_name()

                    if 'Sizer' in name or 'SizeBox' in classname:
                        unreal.log_warning(f"--- {name} ({classname}) ---")

                        # Try to get all numeric properties
                        for prop in ['width_override', 'height_override',
                                    'min_desired_width', 'min_desired_height',
                                    'max_desired_width', 'max_desired_height']:
                            try:
                                val = w.get_editor_property(prop)
                                unreal.log_warning(f"  {prop}: {val}")
                            except:
                                pass

                    # Also check ProgressBar widgets
                    elif 'Bar' in name and 'Progress' in classname:
                        unreal.log_warning(f"--- {name} ({classname}) ---")

                        # Try to get style properties
                        try:
                            style = w.get_editor_property('widget_style')
                            if style:
                                unreal.log_warning(f"  Has widget_style")
                                # Try to get bar fill brush
                                try:
                                    bg_brush = style.get_editor_property('background_image')
                                    if bg_brush:
                                        try:
                                            img_size = bg_brush.get_editor_property('image_size')
                                            unreal.log_warning(f"  background_image size: {img_size}")
                                        except:
                                            pass
                                except:
                                    pass
                                try:
                                    fill_brush = style.get_editor_property('fill_image')
                                    if fill_brush:
                                        try:
                                            img_size = fill_brush.get_editor_property('image_size')
                                            unreal.log_warning(f"  fill_image size: {img_size}")
                                        except:
                                            pass
                                except:
                                    pass
                        except:
                            pass
            else:
                unreal.log_warning("all_widgets is None or empty")
        else:
            unreal.log_warning("Could not get widget_tree")
    except Exception as e:
        unreal.log_error(f"Error: {e}")

    unreal.log_warning("=== Done ===")
