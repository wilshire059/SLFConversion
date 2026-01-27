import unreal
import os
import json

output_file = "C:/scripts/SLFConversion/migration_cache/button_border_brush.json"
os.makedirs(os.path.dirname(output_file), exist_ok=True)

result = {}

bp_path = "/Game/SoulslikeFramework/Widgets/_Generic/W_GenericButton"
bp = unreal.load_asset(bp_path)
if bp:
    result["blueprint_path"] = bp_path

    # Get the widget tree
    try:
        widget_tree = bp.widget_tree
        if widget_tree:
            result["widget_tree_class"] = widget_tree.get_class().get_name()

            # Find ButtonBorder
            all_widgets = widget_tree.all_widgets
            result["widget_count"] = len(all_widgets)
            result["widgets"] = []

            for w in all_widgets:
                widget_info = {
                    "name": w.get_name(),
                    "class": w.get_class().get_name()
                }

                # For Border widgets, extract the Background brush
                if w.get_class().get_name() == "Border":
                    try:
                        background = w.get_editor_property("background")
                        if background:
                            widget_info["background"] = {
                                "draw_as": str(background.draw_as),
                                "tiling": str(background.tiling),
                                "mirroring": str(background.mirroring),
                                "image_size_x": background.image_size.x,
                                "image_size_y": background.image_size.y,
                            }

                            # Tint color
                            tc = background.tint_color.specified_color
                            widget_info["background"]["tint_color"] = {
                                "r": tc.r, "g": tc.g, "b": tc.b, "a": tc.a
                            }
                            widget_info["background"]["color_use_rule"] = str(background.tint_color.color_use_rule)

                            # Outline settings
                            os_data = background.outline_settings
                            widget_info["background"]["outline_settings"] = {
                                "corner_radii_x": os_data.corner_radii.x,
                                "corner_radii_y": os_data.corner_radii.y,
                                "corner_radii_z": os_data.corner_radii.z,
                                "corner_radii_w": os_data.corner_radii.w,
                                "width": os_data.width,
                                "rounding_type": str(os_data.rounding_type),
                                "use_brush_transparency": os_data.b_use_brush_transparency,
                            }
                            # Outline color
                            oc = os_data.color.specified_color
                            widget_info["background"]["outline_settings"]["color"] = {
                                "r": oc.r, "g": oc.g, "b": oc.b, "a": oc.a
                            }

                            # Resource object
                            resource_obj = background.get_resource_object()
                            if resource_obj:
                                widget_info["background"]["resource_object"] = resource_obj.get_path_name()
                            else:
                                widget_info["background"]["resource_object"] = None
                    except Exception as e:
                        widget_info["background_error"] = str(e)

                result["widgets"].append(widget_info)
    except Exception as e:
        result["widget_tree_error"] = str(e)
else:
    result["error"] = f"Could not load {bp_path}"

with open(output_file, "w") as f:
    json.dump(result, f, indent=2)

print(f"Written to {output_file}")
