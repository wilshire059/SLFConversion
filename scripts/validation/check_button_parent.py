import unreal
import os

output_file = "C:/scripts/SLFConversion/migration_cache/button_parent_check.txt"
os.makedirs(os.path.dirname(output_file), exist_ok=True)

lines = []
lines.append("Starting check_button_parent.py")

bp_path = "/Game/SoulslikeFramework/Widgets/_Generic/W_GenericButton"
bp = unreal.load_asset(bp_path)
lines.append(f"Loaded asset: {bp}")

if bp:
    # Try to get parent class from blueprint
    try:
        parent_class = bp.get_editor_property("parent_class")
        lines.append(f"W_GenericButton parent class: {parent_class.get_name()}")
        lines.append(f"Full path: {parent_class.get_path_name()}")
    except Exception as e:
        lines.append(f"Could not get parent_class: {e}")

    gen_class = bp.generated_class()
    if gen_class:
        lines.append(f"Generated class: {gen_class.get_name()}")

        # Also check CDO properties
        cdo = unreal.get_default_object(gen_class)
        lines.append(f"CDO: {cdo.get_name()}")

        # Try to list all properties
        for prop_name in ['button_color', 'selected_button_color', 'outline_color', 'selected_outline_color', 'outline_width', 'outline_corner_radii', 'text_color', 'text_padding']:
            try:
                val = cdo.get_editor_property(prop_name)
                if hasattr(val, 'r'):  # LinearColor
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
                lines.append(f"  {prop_name}: NOT FOUND")
else:
    lines.append("ERROR: Could not load W_GenericButton Blueprint!")

with open(output_file, "w") as f:
    f.write("\n".join(lines))

print(f"Written to {output_file}")
