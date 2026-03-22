import unreal
import os

output_file = "C:/scripts/SLFConversion/migration_cache/button_cdo_check.txt"
os.makedirs(os.path.dirname(output_file), exist_ok=True)

lines = []

bp_path = "/Game/SoulslikeFramework/Widgets/_Generic/W_GenericButton"
bp = unreal.load_asset(bp_path)
if bp:
    gen_class = bp.generated_class()
    if gen_class:
        lines.append(f"Blueprint: {bp_path}")
        lines.append(f"Generated class: {gen_class.get_name()}")
        
        # Get CDO
        cdo = unreal.get_default_object(gen_class)
        lines.append(f"CDO: {cdo.get_name()}")
        
        # Try to list all properties
        props = ['button_color', 'selected_button_color', 'outline_color', 'selected_outline_color', 'outline_width', 'outline_corner_radii', 'text_padding']
        for prop_name in props:
            try:
                val = cdo.get_editor_property(prop_name)
                if hasattr(val, 'r'):  # LinearColor
                    lines.append(f"  {prop_name}: ({val.r:.3f}, {val.g:.3f}, {val.b:.3f}, {val.a:.3f})")
                elif hasattr(val, 'x'):  # Vector4
                    lines.append(f"  {prop_name}: ({val.x:.1f}, {val.y:.1f}, {val.z:.1f}, {val.w:.1f})")
                elif hasattr(val, 'left'):  # Margin
                    lines.append(f"  {prop_name}: (l={val.left:.1f}, t={val.top:.1f}, r={val.right:.1f}, b={val.bottom:.1f})")
                else:
                    lines.append(f"  {prop_name}: {val}")
            except Exception as e:
                lines.append(f"  {prop_name}: NOT FOUND")
                
        # Check widget tree
        try:
            widget_tree = bp.get_editor_property("widget_tree")
            if widget_tree:
                all_widgets = widget_tree.get_editor_property("all_widgets")
                lines.append(f"\nWidget tree has {len(all_widgets)} widgets:")
                for w in all_widgets:
                    wclass = w.get_class().get_name()
                    lines.append(f"  - {w.get_name()} ({wclass})")
                    
                    # For Border, check brush
                    if wclass == "Border":
                        try:
                            brush = w.get_editor_property("background")
                            lines.append(f"      DrawAs: {brush.draw_as}")
                            tc = brush.tint_color.specified_color
                            lines.append(f"      TintColor: ({tc.r:.3f}, {tc.g:.3f}, {tc.b:.3f}, {tc.a:.3f})")
                        except Exception as e:
                            lines.append(f"      Brush error: {e}")
        except Exception as e:
            lines.append(f"Widget tree error: {e}")
else:
    lines.append(f"Could not load {bp_path}")

with open(output_file, "w") as f:
    f.write("\n".join(lines))

print(f"Written to {output_file}")
