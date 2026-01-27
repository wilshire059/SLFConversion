import unreal
import json
import os

# Export all level-up related widgets from bp_only

OUTPUT_DIR = "C:/scripts/SLFConversion/migration_cache/levelup_widgets"
os.makedirs(OUTPUT_DIR, exist_ok=True)

# Widgets to export
LEVELUP_WIDGETS = [
    "/Game/SoulslikeFramework/Widgets/RestMenu/LevelUpMenu/W_LevelCurrencyBlock_LevelUp",
    "/Game/SoulslikeFramework/Widgets/RestMenu/LevelUpMenu/W_LevelUp",
    "/Game/SoulslikeFramework/Widgets/RestMenu/LevelUpMenu/W_StatEntry_LevelUp",
    "/Game/SoulslikeFramework/Widgets/RestMenu/LevelUpMenu/W_StatBlock_LevelUp",
    "/Game/SoulslikeFramework/Widgets/RestMenu/LevelUpMenu/W_LevelUpCost",
    "/Game/SoulslikeFramework/Widgets/RestMenu/LevelUpMenu/W_CurrencyEntry",
    "/Game/SoulslikeFramework/Widgets/RestMenu/LevelUpMenu/W_LevelEntry",
]

def export_widget_blueprint(path):
    """Export widget blueprint details including hierarchy, variables, and bindings."""
    print(f"\n{'='*60}")
    print(f"Exporting: {path}")
    print(f"{'='*60}")

    bp = unreal.EditorAssetLibrary.load_asset(path)
    if not bp:
        print(f"  ERROR: Could not load {path}")
        return None

    result = {
        "path": path,
        "name": bp.get_name(),
        "class": str(type(bp).__name__),
        "variables": [],
        "widget_tree": [],
        "functions": [],
        "event_dispatchers": []
    }

    # Get generated class
    gen_class = None
    if hasattr(bp, 'generated_class'):
        gen_class = bp.generated_class()

    if gen_class:
        result["generated_class"] = str(gen_class.get_name())

        # Get CDO to inspect default values
        cdo = unreal.get_default_object(gen_class)

        # List all properties
        print(f"\n  Properties:")
        for prop_name in dir(cdo):
            if not prop_name.startswith('_') and not callable(getattr(cdo, prop_name, None)):
                try:
                    val = getattr(cdo, prop_name)
                    if val is not None:
                        # Format value for output
                        val_str = str(val)
                        if len(val_str) > 100:
                            val_str = val_str[:100] + "..."
                        print(f"    {prop_name}: {val_str}")
                        result["variables"].append({
                            "name": prop_name,
                            "value": val_str
                        })
                except Exception as e:
                    pass

    # Try to get widget tree
    if hasattr(bp, 'widget_tree') and bp.widget_tree:
        print(f"\n  Widget Tree:")
        root_widget = bp.widget_tree.root_widget
        if root_widget:
            def traverse_widget(widget, depth=0):
                indent = "    " * (depth + 1)
                widget_info = {
                    "name": widget.get_name(),
                    "class": str(type(widget).__name__),
                    "children": []
                }
                print(f"{indent}{widget.get_name()} ({type(widget).__name__})")

                # Get widget properties
                if hasattr(widget, 'slot'):
                    slot = widget.slot
                    if slot:
                        widget_info["slot_class"] = str(type(slot).__name__)

                # Get children if it's a panel widget
                if hasattr(widget, 'get_all_children'):
                    children = widget.get_all_children()
                    for child in children:
                        child_info = traverse_widget(child, depth + 1)
                        widget_info["children"].append(child_info)

                return widget_info

            result["widget_tree"] = traverse_widget(root_widget)

    # Export as text for more details
    export_options = unreal.AssetExportTask()
    export_options.object = bp
    export_options.filename = f"{OUTPUT_DIR}/{bp.get_name()}_export.txt"
    export_options.automated = True
    export_options.prompt = False
    export_options.replace_identical = True

    try:
        success = unreal.Exporter.run_asset_export_task(export_options)
        if success:
            print(f"\n  Exported text to: {export_options.filename}")
    except Exception as e:
        print(f"  Text export failed: {e}")

    return result

def main():
    print("=" * 80)
    print("LEVEL-UP WIDGETS EXPORT")
    print("=" * 80)

    all_results = {}

    for widget_path in LEVELUP_WIDGETS:
        result = export_widget_blueprint(widget_path)
        if result:
            all_results[result["name"]] = result

    # Save JSON summary
    json_path = f"{OUTPUT_DIR}/levelup_widgets_summary.json"
    with open(json_path, 'w') as f:
        json.dump(all_results, f, indent=2, default=str)
    print(f"\n\nSaved summary to: {json_path}")

    # Now specifically examine the W_LevelCurrencyBlock_LevelUp widget
    print("\n" + "=" * 80)
    print("DETAILED ANALYSIS: W_LevelCurrencyBlock_LevelUp")
    print("=" * 80)

    bp_path = "/Game/SoulslikeFramework/Widgets/RestMenu/LevelUpMenu/W_LevelCurrencyBlock_LevelUp"
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)

    if bp and hasattr(bp, 'widget_tree') and bp.widget_tree:
        print("\nWidget Hierarchy (detailed):")

        def print_widget_tree(widget, depth=0):
            indent = "  " * depth
            name = widget.get_name()
            cls = type(widget).__name__

            # Get text content if it's a TextBlock
            text_content = ""
            if cls == "TextBlock":
                try:
                    text_content = f" Text='{widget.get_text()}'"
                except:
                    pass

            print(f"{indent}- {name} ({cls}){text_content}")

            if hasattr(widget, 'get_all_children'):
                for child in widget.get_all_children():
                    print_widget_tree(child, depth + 1)

        print_widget_tree(bp.widget_tree.root_widget)

    print("\n" + "=" * 80)
    print("EXPORT COMPLETE")
    print("=" * 80)

if __name__ == "__main__":
    main()
