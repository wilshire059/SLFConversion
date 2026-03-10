"""
Extract embedded W_Settings_Entry and W_Settings_CategoryEntry widget instances
from the W_Settings WidgetBlueprint by spawning a temporary instance.

Run on bp_only project to get the original configuration.
"""
import unreal
import json
import os

OUTPUT_DIR = "C:/scripts/SLFConversion/migration_cache/"

def extract_widget_properties(widget, widget_name):
    """Extract all relevant properties from a widget instance."""
    data = {"WidgetName": widget_name, "ClassName": str(widget.get_class().get_name())}

    # Properties we care about for Settings widgets
    PROPS_TO_TRY = [
        # W_Settings_Entry properties
        'entry_type', 'EntryType',
        'setting_tag', 'SettingTag',
        'setting_name', 'SettingName',
        'setting_description', 'SettingDescription',
        'button_text', 'ButtonText',
        'unhovered_color', 'UnhoveredColor',
        'hovered_color', 'HoveredColor',
        'current_value', 'CurrentValue',
        # W_Settings_CategoryEntry properties
        'setting_category', 'SettingCategory',
        'switcher_index', 'SwitcherIndex',
        'selected_color', 'SelectedColor',
        'deselected_color', 'DeselectedColor',
        'icon', 'Icon',
        'category_icon', 'CategoryIcon',
    ]

    for prop_name in PROPS_TO_TRY:
        try:
            value = widget.get_editor_property(prop_name)
            if value is not None:
                # Handle different types
                if hasattr(value, 'r') and hasattr(value, 'g') and hasattr(value, 'b'):
                    # LinearColor
                    data[prop_name] = {"R": float(value.r), "G": float(value.g), "B": float(value.b), "A": float(value.a)}
                elif hasattr(value, 'tag_name'):
                    # GameplayTag
                    tag_str = str(value.tag_name) if value.tag_name else ""
                    data[prop_name] = tag_str
                elif hasattr(value, 'value'):
                    # Enum
                    data[prop_name] = int(value.value) if hasattr(value.value, '__int__') else value.value
                elif hasattr(value, 'get_path_name'):
                    # Object reference (texture, etc.)
                    data[prop_name] = str(value.get_path_name())
                elif isinstance(value, (int, float, bool)):
                    data[prop_name] = value
                elif isinstance(value, str):
                    data[prop_name] = value
                else:
                    data[prop_name] = str(value)
        except Exception as e:
            pass

    return data

def get_all_children_recursive(widget, all_widgets):
    """Recursively get all child widgets."""
    if widget is None:
        return

    all_widgets.append(widget)

    # Try different methods to get children
    try:
        if hasattr(widget, 'get_all_children'):
            children = widget.get_all_children()
            for child in children:
                get_all_children_recursive(child, all_widgets)
    except:
        pass

    try:
        if hasattr(widget, 'get_child_at'):
            # For Panel widgets with indexed children
            idx = 0
            while True:
                try:
                    child = widget.get_child_at(idx)
                    if child and child not in all_widgets:
                        get_all_children_recursive(child, all_widgets)
                    idx += 1
                    if idx > 100:  # Safety limit
                        break
                except:
                    break
    except:
        pass

def main():
    results = {
        "Entries": [],
        "Categories": []
    }

    # Load W_Settings WidgetBlueprint
    w_settings_path = "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings"
    wbp = unreal.load_asset(w_settings_path)

    if not wbp:
        unreal.log_warning(f"Failed to load {w_settings_path}")
        return

    unreal.log_warning(f"Loaded W_Settings Blueprint")

    # Get the generated class
    gen_class = wbp.generated_class()
    if not gen_class:
        unreal.log_warning("No generated_class")
        return

    unreal.log_warning(f"Generated class: {gen_class.get_name()}")

    # Create a temporary widget instance
    # We need a world context - use the editor world
    editor_subsystem = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    world = editor_subsystem.get_editor_world()

    if not world:
        unreal.log_warning("No editor world")
        return

    # Create widget
    widget_instance = unreal.WidgetBlueprintLibrary.create(gen_class, world.get_world_settings())

    if not widget_instance:
        unreal.log_warning("Failed to create widget instance")
        return

    unreal.log_warning(f"Created widget instance: {widget_instance.get_name()}")

    # Get all children recursively
    all_widgets = []
    get_all_children_recursive(widget_instance, all_widgets)

    unreal.log_warning(f"Found {len(all_widgets)} widgets")

    for widget in all_widgets:
        if not widget:
            continue

        widget_name = widget.get_name()
        class_name = widget.get_class().get_name()

        # Check if it's a W_Settings_Entry (but not CategoryEntry)
        if 'W_Settings_Entry' in class_name and 'CategoryEntry' not in class_name:
            unreal.log_warning(f"  Entry: {widget_name}")
            entry_data = extract_widget_properties(widget, widget_name)
            results["Entries"].append(entry_data)

            # Log key properties
            for key in ['EntryType', 'entry_type', 'SettingTag', 'setting_tag', 'SettingName', 'setting_name']:
                if key in entry_data and entry_data[key]:
                    unreal.log_warning(f"    {key} = {entry_data[key]}")

        # Check if it's a W_Settings_CategoryEntry
        elif 'W_Settings_CategoryEntry' in class_name:
            unreal.log_warning(f"  Category: {widget_name}")
            cat_data = extract_widget_properties(widget, widget_name)
            results["Categories"].append(cat_data)

            # Log key properties
            for key in ['SettingCategory', 'setting_category', 'SwitcherIndex', 'switcher_index', 'Icon', 'icon']:
                if key in cat_data and cat_data[key]:
                    unreal.log_warning(f"    {key} = {cat_data[key]}")

    # Determine output file based on project
    project_name = str(unreal.Paths.get_project_file_path())
    if "bp_only" in project_name:
        output_file = OUTPUT_DIR + "settings_embedded_bponly.json"
    else:
        output_file = OUTPUT_DIR + "settings_embedded_current.json"

    # Save results
    os.makedirs(OUTPUT_DIR, exist_ok=True)
    with open(output_file, 'w') as f:
        json.dump(results, f, indent=2, default=str)

    unreal.log_warning(f"Saved {len(results['Entries'])} entries, {len(results['Categories'])} categories to {output_file}")

    # Clean up - remove widget
    widget_instance.remove_from_parent()

main()
