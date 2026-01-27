"""
Diagnose W_Settings_CategoryEntry icons at runtime.
Checks visibility, brush, colors, parent widgets.
"""
import unreal
import json
import os

output_dir = "C:/scripts/SLFConversion/migration_cache/settings_comparison"
os.makedirs(output_dir, exist_ok=True)

# Determine project
project_name = unreal.Paths.get_project_file_path()
is_bp_only = "bp_only" in project_name.lower()
project_label = "bp_only" if is_bp_only else "SLFConversion"

print(f"\n{'='*60}")
print(f"Settings Icon Diagnosis - {project_label}")
print(f"{'='*60}\n")

def analyze_widget_tree(widget, depth=0):
    """Recursively analyze widget tree for visibility/opacity issues"""
    indent = "  " * depth
    result = {}

    if not widget:
        return {"error": "null widget"}

    try:
        widget_name = widget.get_name()
        widget_class = widget.get_class().get_name()
        visibility = str(widget.get_visibility())
        render_opacity = widget.get_render_opacity()

        result = {
            "name": widget_name,
            "class": widget_class,
            "visibility": visibility,
            "render_opacity": render_opacity,
        }

        # Check for color and opacity on specific widget types
        if hasattr(widget, 'get_color_and_opacity'):
            try:
                color = widget.get_color_and_opacity()
                result["color_and_opacity"] = f"r:{color.r:.3f} g:{color.g:.3f} b:{color.b:.3f} a:{color.a:.3f}"
            except:
                pass

        # Check for brush on Image widgets
        if hasattr(widget, 'brush') or hasattr(widget, 'get_brush'):
            try:
                if hasattr(widget, 'brush'):
                    brush = widget.brush
                else:
                    brush = widget.get_editor_property('brush')

                if brush:
                    result["brush_draw_as"] = str(brush.get_editor_property('draw_as'))
                    result["brush_tint_color"] = str(brush.get_editor_property('tint_color'))
                    result["brush_image_size"] = str(brush.get_editor_property('image_size'))

                    # Get resource
                    resource = brush.get_resource_object()
                    if resource:
                        result["brush_resource"] = resource.get_path_name()
                    else:
                        result["brush_resource"] = "NONE"
            except Exception as e:
                result["brush_error"] = str(e)

        print(f"{indent}{widget_name} ({widget_class}): vis={visibility}, opacity={render_opacity}")

    except Exception as e:
        result["error"] = str(e)
        print(f"{indent}Error analyzing widget: {e}")

    return result

# Load W_Settings widget blueprint
settings_path = "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings"
settings_bp = unreal.load_asset(settings_path)

if not settings_bp:
    print("ERROR: Could not load W_Settings")
else:
    print(f"Loaded W_Settings: {settings_bp}")

    # Get generated class and CDO
    try:
        gen_class = settings_bp.get_editor_property('generated_class')
        print(f"Generated class: {gen_class}")

        if gen_class:
            cdo = unreal.get_default_object(gen_class)
            print(f"CDO: {cdo}")

            # Try to get W_Settings_CategoryEntry references
            # These are usually named widgets in the tree
    except Exception as e:
        print(f"Error getting generated class: {e}")

# Load W_Settings_CategoryEntry
entry_path = "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_CategoryEntry"
entry_bp = unreal.load_asset(entry_path)

diagnosis_result = {
    "project": project_label,
    "entry_analysis": {}
}

if not entry_bp:
    print("ERROR: Could not load W_Settings_CategoryEntry")
else:
    print(f"\nLoaded W_Settings_CategoryEntry: {entry_bp}")

    try:
        gen_class = entry_bp.get_editor_property('generated_class')
        print(f"Generated class: {gen_class}")

        # Check parent class
        if gen_class:
            parent = gen_class.get_super_class() if hasattr(gen_class, 'get_super_class') else None
            if parent:
                print(f"Parent class: {parent.get_name()}")
                diagnosis_result["entry_analysis"]["parent_class"] = parent.get_name()

            # Get CDO and check properties
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                print(f"\nCDO Properties:")

                # Check Icon property
                try:
                    icon = cdo.get_editor_property('icon')
                    print(f"  icon: {icon}")
                    diagnosis_result["entry_analysis"]["icon"] = str(icon)
                except Exception as e:
                    print(f"  icon: ERROR - {e}")

                # Check colors
                try:
                    selected_color = cdo.get_editor_property('selected_color')
                    print(f"  selected_color: r={selected_color.r:.3f} g={selected_color.g:.3f} b={selected_color.b:.3f} a={selected_color.a:.3f}")
                    diagnosis_result["entry_analysis"]["selected_color"] = f"r={selected_color.r:.3f} g={selected_color.g:.3f} b={selected_color.b:.3f} a={selected_color.a:.3f}"
                except Exception as e:
                    print(f"  selected_color: ERROR - {e}")

                try:
                    deselected_color = cdo.get_editor_property('deselected_color')
                    print(f"  deselected_color: r={deselected_color.r:.3f} g={deselected_color.g:.3f} b={deselected_color.b:.3f} a={deselected_color.a:.3f}")
                    diagnosis_result["entry_analysis"]["deselected_color"] = f"r={deselected_color.r:.3f} g={deselected_color.g:.3f} b={deselected_color.b:.3f} a={deselected_color.a:.3f}"
                except Exception as e:
                    print(f"  deselected_color: ERROR - {e}")

                # Check Selected
                try:
                    selected = cdo.get_editor_property('selected')
                    print(f"  selected: {selected}")
                    diagnosis_result["entry_analysis"]["selected"] = str(selected)
                except Exception as e:
                    print(f"  selected: ERROR - {e}")

                # Check SwitcherIndex
                try:
                    switcher_index = cdo.get_editor_property('switcher_index')
                    print(f"  switcher_index: {switcher_index}")
                    diagnosis_result["entry_analysis"]["switcher_index"] = str(switcher_index)
                except Exception as e:
                    print(f"  switcher_index: ERROR - {e}")

    except Exception as e:
        print(f"Error analyzing: {e}")
        diagnosis_result["entry_analysis"]["error"] = str(e)

# Check widget tree from widget blueprint
print(f"\n{'='*60}")
print("Widget Tree Analysis")
print(f"{'='*60}\n")

if entry_bp:
    try:
        # Get the widget tree
        widget_tree = entry_bp.get_editor_property('widget_tree')
        if widget_tree:
            print(f"Widget tree found: {widget_tree}")

            # Get root widget
            root = widget_tree.get_editor_property('root_widget')
            if root:
                print(f"Root widget: {root.get_name()}")
                diagnosis_result["widget_tree"] = {"root": root.get_name()}

                # Get all widgets
                all_widgets = widget_tree.get_editor_property('all_widgets')
                print(f"All widgets count: {len(all_widgets) if all_widgets else 0}")

                if all_widgets:
                    widgets_info = []
                    for w in all_widgets:
                        w_name = w.get_name()
                        w_class = w.get_class().get_name()
                        print(f"  - {w_name} ({w_class})")

                        widget_data = {"name": w_name, "class": w_class}

                        # Check Image widgets for brush info
                        if "Image" in w_class:
                            try:
                                brush = w.get_editor_property('brush')
                                if brush:
                                    resource = brush.get_resource_object()
                                    widget_data["brush_resource"] = resource.get_path_name() if resource else "NONE"
                                    widget_data["brush_image_size"] = str(brush.get_editor_property('image_size'))
                                    widget_data["brush_draw_as"] = str(brush.get_editor_property('draw_as'))

                                    tint = brush.get_editor_property('tint_color')
                                    if tint:
                                        spec_color = tint.get_editor_property('specified_color')
                                        widget_data["brush_tint"] = f"r={spec_color.r:.3f} g={spec_color.g:.3f} b={spec_color.b:.3f} a={spec_color.a:.3f}"

                                    print(f"      brush_resource: {widget_data.get('brush_resource', 'unknown')}")
                                    print(f"      brush_size: {widget_data.get('brush_image_size', 'unknown')}")
                                    print(f"      brush_tint: {widget_data.get('brush_tint', 'unknown')}")
                            except Exception as e:
                                widget_data["brush_error"] = str(e)

                        # Check visibility
                        try:
                            vis = w.get_editor_property('visibility')
                            widget_data["visibility"] = str(vis)
                        except:
                            pass

                        # Check render opacity
                        try:
                            opacity = w.get_editor_property('render_opacity')
                            widget_data["render_opacity"] = str(opacity)
                        except:
                            pass

                        # Check color and opacity for images
                        try:
                            color_opacity = w.get_editor_property('color_and_opacity')
                            if color_opacity:
                                widget_data["color_and_opacity"] = f"r={color_opacity.r:.3f} g={color_opacity.g:.3f} b={color_opacity.b:.3f} a={color_opacity.a:.3f}"
                                print(f"      color_and_opacity: {widget_data['color_and_opacity']}")
                        except:
                            pass

                        widgets_info.append(widget_data)

                    diagnosis_result["widget_tree"]["widgets"] = widgets_info
    except Exception as e:
        print(f"Error analyzing widget tree: {e}")
        diagnosis_result["widget_tree_error"] = str(e)

# Save results
output_file = os.path.join(output_dir, f"icon_diagnosis_{project_label}.json")
with open(output_file, 'w') as f:
    json.dump(diagnosis_result, f, indent=2)
print(f"\nSaved diagnosis to: {output_file}")

print(f"\n{'='*60}")
print("Diagnosis Complete")
print(f"{'='*60}\n")
