import unreal
import os

output_file = "C:/scripts/SLFConversion/migration_cache/button_complete_check.txt"
os.makedirs(os.path.dirname(output_file), exist_ok=True)

lines = []

bp_path = "/Game/SoulslikeFramework/Widgets/_Generic/W_GenericButton"
bp = unreal.load_asset(bp_path)

if bp:
    lines.append(f"Blueprint: {bp_path}")
    lines.append(f"Blueprint Class: {bp.get_class().get_name()}")

    gen_class = bp.generated_class()
    if gen_class:
        lines.append(f"Generated class: {gen_class.get_name()}")

        # Get CDO
        cdo = unreal.get_default_object(gen_class)
        lines.append(f"CDO: {cdo.get_name()}")

        # Get parent chain by walking classes up
        lines.append("\nClass Hierarchy:")
        current_class = gen_class
        depth = 0
        while current_class:
            class_name = current_class.get_name()
            class_path = current_class.get_path_name()
            lines.append(f"  {'  ' * depth}{class_name} ({class_path})")
            # Try to get outer class
            try:
                # Use static class method for native classes
                if "W_GenericButton" in class_name:
                    # This is our generated class
                    pass
                elif hasattr(current_class, 'static_class'):
                    pass
            except:
                pass
            depth += 1
            if depth > 10 or class_name in ['Object', 'UObject']:
                break
            # Can't easily get parent in Python API, so break after first
            break

        # Check the widget tree
        lines.append("\n--- Widget Tree ---")
        try:
            widget_tree = bp.widget_tree
            if widget_tree:
                all_widgets = widget_tree.all_widgets
                lines.append(f"Widget count: {len(all_widgets)}")

                for w in all_widgets:
                    widget_name = w.get_name()
                    widget_class = w.get_class().get_name()
                    lines.append(f"\n  Widget: {widget_name} ({widget_class})")

                    # For Border widgets, get the background brush
                    if widget_class == "Border":
                        try:
                            bg = w.get_editor_property("background")
                            lines.append(f"    Background.DrawAs: {bg.draw_as}")
                            lines.append(f"    Background.Tiling: {bg.tiling}")
                            tc = bg.tint_color
                            sc = tc.specified_color
                            lines.append(f"    Background.TintColor: ({sc.r:.3f}, {sc.g:.3f}, {sc.b:.3f}, {sc.a:.3f})")
                            lines.append(f"    Background.ColorUseRule: {tc.color_use_rule}")

                            # Outline settings
                            os_data = bg.outline_settings
                            lines.append(f"    OutlineSettings.Width: {os_data.width}")
                            lines.append(f"    OutlineSettings.CornerRadii: ({os_data.corner_radii.x}, {os_data.corner_radii.y}, {os_data.corner_radii.z}, {os_data.corner_radii.w})")
                            lines.append(f"    OutlineSettings.RoundingType: {os_data.rounding_type}")
                            oc = os_data.color.specified_color
                            lines.append(f"    OutlineSettings.Color: ({oc.r:.3f}, {oc.g:.3f}, {oc.b:.3f}, {oc.a:.3f})")

                            # Resource object
                            res = bg.get_resource_object()
                            if res:
                                lines.append(f"    Background.ResourceObject: {res.get_path_name()}")
                            else:
                                lines.append(f"    Background.ResourceObject: None")
                        except Exception as e:
                            lines.append(f"    Background error: {e}")
            else:
                lines.append("Widget tree is None")
        except Exception as e:
            lines.append(f"Widget tree error: {e}")

        # Check CDO properties
        lines.append("\n--- CDO Properties ---")
        for prop_name in ['button_color', 'selected_button_color', 'outline_color', 'selected_outline_color', 'outline_width', 'outline_corner_radii', 'text_color', 'text_padding', 'text', 'selected']:
            try:
                val = cdo.get_editor_property(prop_name)
                if hasattr(val, 'r') and not hasattr(val, 'specified_color'):  # LinearColor
                    lines.append(f"  {prop_name}: ({val.r:.3f}, {val.g:.3f}, {val.b:.3f}, {val.a:.3f})")
                elif hasattr(val, 'x'):  # Vector4
                    lines.append(f"  {prop_name}: ({val.x:.1f}, {val.y:.1f}, {val.z:.1f}, {val.w:.1f})")
                elif hasattr(val, 'left'):  # Margin
                    lines.append(f"  {prop_name}: (l={val.left:.1f}, t={val.top:.1f}, r={val.right:.1f}, b={val.bottom:.1f})")
                elif hasattr(val, 'specified_color'):  # SlateColor
                    sc = val.specified_color
                    lines.append(f"  {prop_name}: SlateColor({sc.r:.3f}, {sc.g:.3f}, {sc.b:.3f}, {sc.a:.3f})")
                else:
                    lines.append(f"  {prop_name}: {val}")
            except Exception as e:
                lines.append(f"  {prop_name}: NOT FOUND ({e})")
else:
    lines.append(f"ERROR: Could not load {bp_path}")

with open(output_file, "w") as f:
    f.write("\n".join(lines))

print(f"Written to {output_file}")
