"""
Export W_Settings related blueprints from bp_only for comparison
"""
import unreal
import json
import os

OUTPUT_DIR = "C:/scripts/SLFConversion/migration_cache/bp_only_settings_exports"

WIDGETS_TO_EXPORT = [
    "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings",
    "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_Entry",
    "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_PlayerCard",
    "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_CategoryEntry",
    "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_CenteredText",
]

def export_widget_info(bp_path):
    """Export key information about a widget blueprint"""
    bp = unreal.load_asset(bp_path)
    if not bp:
        return None

    info = {
        "Name": bp.get_name(),
        "Path": bp_path,
        "ParentClass": None,
        "Variables": [],
        "Functions": [],
        "EventDispatchers": [],
    }

    # Get parent class
    if hasattr(bp, 'parent_class') and bp.parent_class:
        info["ParentClass"] = bp.parent_class.get_name()

    # Get variables
    if hasattr(bp, 'new_variables'):
        for var in bp.new_variables:
            var_info = {
                "Name": str(var.var_name),
                "Type": str(var.var_type.pin_category) if hasattr(var, 'var_type') else "unknown",
            }
            info["Variables"].append(var_info)

    # Try to get function names from graphs
    if hasattr(bp, 'function_graphs'):
        for graph in bp.function_graphs:
            if hasattr(graph, 'get_name'):
                info["Functions"].append(graph.get_name())

    # Get uber graph pages (event graphs)
    if hasattr(bp, 'uber_graph_pages'):
        for page in bp.uber_graph_pages:
            if hasattr(page, 'get_name'):
                info["Functions"].append(f"EventGraph:{page.get_name()}")

    return info

def main():
    os.makedirs(OUTPUT_DIR, exist_ok=True)

    unreal.log_warning("=" * 60)
    unreal.log_warning("EXPORTING BP_ONLY SETTINGS WIDGETS")
    unreal.log_warning("=" * 60)

    all_exports = {}

    for bp_path in WIDGETS_TO_EXPORT:
        unreal.log_warning(f"Exporting: {bp_path}")
        info = export_widget_info(bp_path)
        if info:
            all_exports[info["Name"]] = info
            unreal.log_warning(f"  Parent: {info['ParentClass']}")
            unreal.log_warning(f"  Variables: {len(info['Variables'])}")
            unreal.log_warning(f"  Functions: {len(info['Functions'])}")
        else:
            unreal.log_error(f"  Failed to load!")

    # Save to JSON
    output_file = os.path.join(OUTPUT_DIR, "settings_widgets_summary.json")
    with open(output_file, 'w') as f:
        json.dump(all_exports, f, indent=2)

    unreal.log_warning(f"Saved to: {output_file}")
    unreal.log_warning("=" * 60)

main()
