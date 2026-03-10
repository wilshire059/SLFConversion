"""
Extract all W_Settings_Entry instance properties from the W_Settings widget.
This extracts the per-widget configuration (EntryType, SettingName, SettingTag, etc.)
Run on bp_only project to get correct values.
"""
import unreal
import json
import os

OUTPUT_DIR = "C:/scripts/SLFConversion/migration_cache/"

def extract_entry_properties(entry_widget, entry_name):
    """Extract all relevant properties from a W_Settings_Entry instance."""
    data = {"name": entry_name}

    # Properties to extract
    PROPS = [
        'entry_type', 'EntryType',
        'setting_tag', 'SettingTag',
        'setting_name', 'SettingName',
        'setting_description', 'SettingDescription',
        'button_text', 'ButtonText',
        'unhovered_color', 'UnhoveredColor',
        'hovered_color', 'HoveredColor',
        'current_value', 'CurrentValue',
    ]

    for prop_name in PROPS:
        try:
            value = entry_widget.get_editor_property(prop_name)
            if value is not None:
                # Handle different types
                if hasattr(value, 'r') and hasattr(value, 'g') and hasattr(value, 'b'):
                    data[prop_name] = {"R": float(value.r), "G": float(value.g), "B": float(value.b), "A": float(value.a)}
                elif hasattr(value, 'tag_name'):
                    data[prop_name] = str(value.tag_name)
                elif hasattr(value, 'value'):  # Enum
                    data[prop_name] = int(value.value)
                elif isinstance(value, (int, float, bool)):
                    data[prop_name] = value
                else:
                    data[prop_name] = str(value)
        except:
            pass

    return data

def extract_category_properties(cat_widget, cat_name):
    """Extract properties from a W_Settings_CategoryEntry instance."""
    data = {"name": cat_name}

    PROPS = [
        'setting_category', 'SettingCategory',
        'switcher_index', 'SwitcherIndex',
        'selected_color', 'SelectedColor',
        'deselected_color', 'DeselectedColor',
        'category_icon', 'CategoryIcon',
    ]

    for prop_name in PROPS:
        try:
            value = cat_widget.get_editor_property(prop_name)
            if value is not None:
                if hasattr(value, 'r') and hasattr(value, 'g') and hasattr(value, 'b'):
                    data[prop_name] = {"R": float(value.r), "G": float(value.g), "B": float(value.b), "A": float(value.a)}
                elif hasattr(value, 'tag_name'):
                    data[prop_name] = str(value.tag_name)
                elif isinstance(value, (int, float, bool)):
                    data[prop_name] = value
                elif value is not None:
                    # Could be a texture/icon
                    data[prop_name] = str(value.get_path_name()) if hasattr(value, 'get_path_name') else str(value)
        except:
            pass

    return data

def main():
    results = {
        "entries": [],
        "categories": []
    }

    # Load W_Settings widget
    w_settings_path = "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings"
    bp = unreal.load_asset(w_settings_path)
    if not bp:
        unreal.log_warning(f"Failed to load {w_settings_path}")
        return

    unreal.log_warning("=== Extracting W_Settings Entry Data ===")

    # Get the widget tree
    gen_class = bp.generated_class()
    if not gen_class:
        unreal.log_warning("No generated class")
        return

    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        unreal.log_warning("No CDO")
        return

    # Get the widget tree from the WidgetBlueprint
    widget_tree = bp.get_editor_property('widget_tree')
    if not widget_tree:
        unreal.log_warning("No widget_tree")
        return

    # Get all widgets
    all_widgets = widget_tree.get_editor_property('all_widgets')
    if not all_widgets:
        unreal.log_warning("No all_widgets")
        return

    unreal.log_warning(f"Found {len(all_widgets)} widgets")

    for widget in all_widgets:
        if not widget:
            continue

        widget_name = widget.get_name()
        widget_class = widget.get_class().get_name()

        # Check if it's a W_Settings_Entry
        if 'W_Settings_Entry_C' in widget_class and 'CategoryEntry' not in widget_name:
            unreal.log_warning(f"  Entry: {widget_name}")
            entry_data = extract_entry_properties(widget, widget_name)
            results["entries"].append(entry_data)

        # Check if it's a W_Settings_CategoryEntry
        elif 'W_Settings_CategoryEntry_C' in widget_class:
            unreal.log_warning(f"  Category: {widget_name}")
            cat_data = extract_category_properties(widget, widget_name)
            results["categories"].append(cat_data)

    # Determine output file based on project
    project_name = str(unreal.Paths.get_project_file_path())
    if "bp_only" in project_name:
        output_file = OUTPUT_DIR + "settings_entries_bponly.json"
    else:
        output_file = OUTPUT_DIR + "settings_entries_current.json"

    # Save results
    os.makedirs(OUTPUT_DIR, exist_ok=True)
    with open(output_file, 'w') as f:
        json.dump(results, f, indent=2, default=str)

    unreal.log_warning(f"=== Saved {len(results['entries'])} entries and {len(results['categories'])} categories to {output_file} ===")

main()
