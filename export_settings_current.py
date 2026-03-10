"""
Export W_Settings widget configuration from CURRENT project for comparison to bp_only
"""
import unreal
import json
import os

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/settings_current_export.json"

def get_widget_property(widget, prop_name, default=None):
    """Safely get a property from a widget"""
    try:
        return widget.get_editor_property(prop_name)
    except:
        return default

def color_to_dict(color):
    """Convert FLinearColor to dict"""
    if color is None:
        return None
    return {
        "R": color.r,
        "G": color.g,
        "B": color.b,
        "A": color.a
    }

def tag_to_string(tag):
    """Convert FGameplayTag to string"""
    if tag is None:
        return ""
    return str(tag.tag_name) if hasattr(tag, 'tag_name') else str(tag)

def export_settings_widget():
    """Export W_Settings widget tree and embedded widget properties"""

    # Load the W_Settings widget blueprint
    widget_path = "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings"
    widget_bp = unreal.load_asset(widget_path)

    if not widget_bp:
        unreal.log_error(f"Failed to load: {widget_path}")
        return None

    # Get parent class via generated_class
    parent_class_name = "None"
    try:
        gen_class = widget_bp.generated_class()
        if gen_class:
            parent = gen_class.get_super_class()
            if parent:
                parent_class_name = parent.get_name()
    except:
        pass

    result = {
        "WidgetPath": widget_path,
        "ParentClass": parent_class_name,
        "Categories": [],
        "Entries": [],
        "CategorySwitcher": None
    }

    # Get widget tree via get_editor_property
    try:
        widget_tree = widget_bp.get_editor_property("widget_tree")
    except:
        widget_tree = None

    if not widget_tree:
        unreal.log_error("No widget tree found")
        return result

    # Find all widgets in the tree
    try:
        all_widgets = widget_tree.get_editor_property("all_widgets")
    except:
        all_widgets = []
    unreal.log_warning(f"Found {len(all_widgets)} widgets in tree")

    # Find CategorySwitcher
    for widget in all_widgets:
        widget_name = widget.get_name()
        widget_class = widget.get_class().get_name()

        if widget_name == "CategorySwitcher" or "WidgetSwitcher" in widget_class:
            # Get number of children/slots
            num_widgets = 0
            try:
                num_widgets = widget.get_num_widgets() if hasattr(widget, 'get_num_widgets') else 0
            except:
                pass

            result["CategorySwitcher"] = {
                "Name": widget_name,
                "Class": widget_class,
                "NumWidgets": num_widgets
            }

            # Try to get child names
            try:
                children = []
                for i in range(num_widgets):
                    child = widget.get_widget_at_index(i)
                    if child:
                        children.append({
                            "Index": i,
                            "Name": child.get_name(),
                            "Class": child.get_class().get_name()
                        })
                result["CategorySwitcher"]["Children"] = children
            except Exception as e:
                unreal.log_warning(f"Could not get switcher children: {e}")

    # Extract category entries
    for widget in all_widgets:
        widget_name = widget.get_name()
        widget_class = widget.get_class().get_name()

        # Check for category entries
        if "CategoryEntry" in widget_name or "CategoryEntry" in widget_class:
            cat_info = {
                "WidgetName": widget_name,
                "ClassName": widget_class,
                "SwitcherIndex": get_widget_property(widget, "switcher_index", -999),
                "SettingCategory": tag_to_string(get_widget_property(widget, "setting_category")),
                "SelectedColor": color_to_dict(get_widget_property(widget, "selected_color")),
                "DeselectedColor": color_to_dict(get_widget_property(widget, "deselected_color")),
            }

            # Try to get icon
            icon = get_widget_property(widget, "icon")
            if icon:
                cat_info["Icon"] = str(icon.get_path_name()) if hasattr(icon, 'get_path_name') else str(icon)

            result["Categories"].append(cat_info)
            unreal.log_warning(f"Category: {widget_name} -> SwitcherIndex={cat_info['SwitcherIndex']}")

    # Extract setting entries
    for widget in all_widgets:
        widget_name = widget.get_name()
        widget_class = widget.get_class().get_name()

        # Check for setting entries (but not category entries)
        if ("Entry" in widget_class or "Entry" in widget_name) and "Category" not in widget_name and "Category" not in widget_class:
            entry_info = {
                "WidgetName": widget_name,
                "ClassName": widget_class,
                "SettingTag": tag_to_string(get_widget_property(widget, "setting_tag")),
                "SettingName": str(get_widget_property(widget, "setting_name", "")),
                "EntryType": int(get_widget_property(widget, "entry_type", 0)) if get_widget_property(widget, "entry_type") is not None else -1,
                "UnhoveredColor": color_to_dict(get_widget_property(widget, "unhovered_color")),
                "HoveredColor": color_to_dict(get_widget_property(widget, "hovered_color")),
            }
            result["Entries"].append(entry_info)
            unreal.log_warning(f"Entry: {widget_name} -> Tag={entry_info['SettingTag']}, Type={entry_info['EntryType']}")

    return result

def main():
    os.makedirs(os.path.dirname(OUTPUT_FILE), exist_ok=True)

    unreal.log_warning("=" * 60)
    unreal.log_warning("EXPORTING CURRENT W_SETTINGS CONFIGURATION")
    unreal.log_warning("=" * 60)

    result = export_settings_widget()

    if result:
        with open(OUTPUT_FILE, 'w') as f:
            json.dump(result, f, indent=2)
        unreal.log_warning(f"Saved to: {OUTPUT_FILE}")
        unreal.log_warning(f"Categories: {len(result['Categories'])}")
        unreal.log_warning(f"Entries: {len(result['Entries'])}")
    else:
        unreal.log_error("Export failed!")

    unreal.log_warning("=" * 60)

main()
