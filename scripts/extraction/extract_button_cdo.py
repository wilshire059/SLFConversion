import unreal
import json
import os

# Load W_GenericButton and examine its widget tree and CDO
bp_path = "/Game/SoulslikeFramework/Widgets/_Generic/W_GenericButton"
output_path = "C:/scripts/SLFConversion/migration_cache/w_generic_button_full.json"

bp = unreal.load_asset(bp_path)
if not bp:
    print(f"Could not load {bp_path}")
    exit()

result = {
    "blueprint_path": bp_path,
    "widget_tree_info": {},
    "variables": {}
}

print(f"Loaded: {bp_path}")
print(f"Blueprint class: {bp.get_class().get_name()}")

# Get widget tree
try:
    widget_tree = bp.get_editor_property("widget_tree")
    if widget_tree:
        print(f"Widget tree: {widget_tree.get_name()}")
        
        # Get root widget
        root = widget_tree.get_editor_property("root_widget")
        if root:
            print(f"Root widget: {root.get_name()} ({root.get_class().get_name()})")
            
            # Find all widgets recursively
            def get_all_children(widget, depth=0):
                widgets_info = []
                if not widget:
                    return widgets_info
                    
                info = {
                    "name": widget.get_name(),
                    "class": widget.get_class().get_name(),
                    "depth": depth
                }
                
                # Check for Border specifically
                if widget.get_class().get_name() == "Border":
                    try:
                        brush = widget.get_editor_property("background")
                        if brush:
                            info["brush_draw_as"] = str(brush.draw_as)
                            info["brush_tint_color"] = {
                                "r": brush.tint_color.specified_color.r,
                                "g": brush.tint_color.specified_color.g,
                                "b": brush.tint_color.specified_color.b,
                                "a": brush.tint_color.specified_color.a
                            }
                            print(f"  Border {widget.get_name()}: DrawAs={brush.draw_as}, TintColor=({brush.tint_color.specified_color.r:.2f}, {brush.tint_color.specified_color.g:.2f}, {brush.tint_color.specified_color.b:.2f}, {brush.tint_color.specified_color.a:.2f})")
                    except Exception as e:
                        print(f"  Error reading border: {e}")
                
                widgets_info.append(info)
                
                # Try to get children
                try:
                    if hasattr(widget, 'get_child_at'):
                        # Panel widget
                        num_children = widget.get_child_count() if hasattr(widget, 'get_child_count') else 0
                        for i in range(num_children):
                            child = widget.get_child_at(i)
                            if child:
                                widgets_info.extend(get_all_children(child, depth + 1))
                except:
                    pass
                
                # ContentWidget for Border/other containers
                try:
                    content = widget.get_editor_property("content")
                    if content:
                        widgets_info.extend(get_all_children(content, depth + 1))
                except:
                    pass
                    
                return widgets_info
            
            result["widget_tree_info"]["widgets"] = get_all_children(root)
            
            # Now iterate through all widget tree nodes
            all_widgets = widget_tree.get_editor_property("all_widgets")
            if all_widgets:
                print(f"\nAll widgets in tree ({len(all_widgets)}):")
                for w in all_widgets:
                    class_name = w.get_class().get_name()
                    print(f"  - {w.get_name()} ({class_name})")
                    
                    if class_name == "Border":
                        try:
                            brush = w.get_editor_property("background")
                            if brush:
                                print(f"      Background: DrawAs={brush.draw_as}")
                                print(f"      TintColor: ({brush.tint_color.specified_color.r:.3f}, {brush.tint_color.specified_color.g:.3f}, {brush.tint_color.specified_color.b:.3f}, {brush.tint_color.specified_color.a:.3f})")
                                if hasattr(brush, 'outline_settings'):
                                    outline = brush.outline_settings
                                    print(f"      OutlineWidth: {outline.width}")
                                    print(f"      CornerRadii: ({outline.corner_radii.x}, {outline.corner_radii.y}, {outline.corner_radii.z}, {outline.corner_radii.w})")
                        except Exception as e:
                            print(f"      Error: {e}")
except Exception as e:
    print(f"Error accessing widget tree: {e}")

os.makedirs(os.path.dirname(output_path), exist_ok=True)
with open(output_path, "w") as f:
    json.dump(result, f, indent=2, default=str)
print(f"\nSaved to: {output_path}")
