import unreal
import json
import os

def extract_widget_config(widget_path, output_name):
    """Extract complete widget configuration including CDO, components, variables"""
    
    result = {
        "path": widget_path,
        "variables": {},
        "components": {},
        "cdo_properties": {},
        "widget_tree": []
    }
    
    # Load the widget Blueprint
    bp = unreal.load_asset(widget_path)
    if not bp:
        print(f"ERROR: Could not load {widget_path}")
        return None
    
    print(f"Loaded: {widget_path}")
    
    # Get the generated class
    gen_class = bp.generated_class()
    if not gen_class:
        print(f"ERROR: No generated class for {widget_path}")
        return None
    
    # Get CDO
    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        print(f"ERROR: No CDO for {widget_path}")
        return None
    
    print(f"Got CDO: {cdo.get_name()}")
    
    # Extract key properties from CDO
    try:
        # Text
        text_val = cdo.get_editor_property("text")
        if text_val:
            result["cdo_properties"]["text"] = str(text_val)
    except:
        pass
    
    try:
        # TextColor
        text_color = cdo.get_editor_property("text_color")
        if text_color:
            spec_color = text_color.get_specified_color()
            result["cdo_properties"]["text_color"] = {
                "r": spec_color.r, "g": spec_color.g, "b": spec_color.b, "a": spec_color.a
            }
    except:
        pass
    
    try:
        # ButtonColor
        button_color = cdo.get_editor_property("button_color")
        if button_color:
            result["cdo_properties"]["button_color"] = {
                "r": button_color.r, "g": button_color.g, "b": button_color.b, "a": button_color.a
            }
    except:
        pass
    
    try:
        # SelectedButtonColor
        sel_button_color = cdo.get_editor_property("selected_button_color")
        if sel_button_color:
            result["cdo_properties"]["selected_button_color"] = {
                "r": sel_button_color.r, "g": sel_button_color.g, "b": sel_button_color.b, "a": sel_button_color.a
            }
    except:
        pass
    
    try:
        # OutlineColor
        outline_color = cdo.get_editor_property("outline_color")
        if outline_color:
            result["cdo_properties"]["outline_color"] = {
                "r": outline_color.r, "g": outline_color.g, "b": outline_color.b, "a": outline_color.a
            }
    except:
        pass
    
    try:
        # SelectedOutlineColor
        sel_outline_color = cdo.get_editor_property("selected_outline_color")
        if sel_outline_color:
            result["cdo_properties"]["selected_outline_color"] = {
                "r": sel_outline_color.r, "g": sel_outline_color.g, "b": sel_outline_color.b, "a": sel_outline_color.a
            }
    except:
        pass
    
    try:
        # OutlineWidth
        result["cdo_properties"]["outline_width"] = cdo.get_editor_property("outline_width")
    except:
        pass
    
    try:
        # OutlineCornerRadii
        corner_radii = cdo.get_editor_property("outline_corner_radii")
        if corner_radii:
            result["cdo_properties"]["outline_corner_radii"] = {
                "x": corner_radii.x, "y": corner_radii.y, "z": corner_radii.z, "w": corner_radii.w
            }
    except:
        pass
    
    try:
        # TextPadding
        text_padding = cdo.get_editor_property("text_padding")
        if text_padding:
            result["cdo_properties"]["text_padding"] = {
                "left": text_padding.left, "top": text_padding.top,
                "right": text_padding.right, "bottom": text_padding.bottom
            }
    except:
        pass
    
    try:
        # Selected
        result["cdo_properties"]["selected"] = cdo.get_editor_property("selected")
    except:
        pass
    
    # Try to get widget tree from the Blueprint
    try:
        widget_tree = bp.get_editor_property("widget_tree")
        if widget_tree:
            root = widget_tree.get_editor_property("root_widget")
            if root:
                result["widget_tree"] = extract_widget_hierarchy(root, 0)
    except Exception as e:
        print(f"Could not extract widget tree: {e}")
    
    return result

def extract_widget_hierarchy(widget, depth):
    """Recursively extract widget hierarchy"""
    if not widget:
        return []
    
    widgets = []
    
    widget_info = {
        "name": widget.get_name(),
        "class": widget.get_class().get_name(),
        "depth": depth
    }
    
    # Try to get specific properties based on widget type
    class_name = widget.get_class().get_name()
    
    if class_name == "Border":
        try:
            brush = widget.get_editor_property("background")
            if brush:
                widget_info["brush_draw_as"] = str(brush.get_editor_property("draw_as"))
                tint = brush.get_editor_property("tint_color")
                if tint:
                    spec = tint.get_specified_color()
                    widget_info["brush_tint"] = {"r": spec.r, "g": spec.g, "b": spec.b, "a": spec.a}
        except:
            pass
    
    if class_name == "TextBlock":
        try:
            widget_info["text"] = str(widget.get_editor_property("text"))
        except:
            pass
    
    widgets.append(widget_info)
    
    # Get children
    try:
        # For Panel widgets
        if hasattr(widget, "get_all_children"):
            children = widget.get_all_children()
            for child in children:
                widgets.extend(extract_widget_hierarchy(child, depth + 1))
    except:
        pass
    
    return widgets

# Extract from bp_only
bp_only_path = "/Game/SoulslikeFramework/Widgets/_Generic/W_GenericButton"
bp_only_config = extract_widget_config(bp_only_path, "bp_only")

if bp_only_config:
    output_path = "C:/scripts/SLFConversion/migration_cache/w_generic_button_config.json"
    os.makedirs(os.path.dirname(output_path), exist_ok=True)
    with open(output_path, "w") as f:
        json.dump(bp_only_config, f, indent=2, default=str)
    print(f"\nSaved config to: {output_path}")
    print(json.dumps(bp_only_config, indent=2, default=str))
else:
    print("Failed to extract config")
