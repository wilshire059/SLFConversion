"""
Simple settings data extraction using Python reflection.
Works on both bp_only and SLFConversion.
"""
import unreal
import json
import os

OUTPUT_DIR = "C:/scripts/SLFConversion/migration_cache/"

WIDGETS = [
    "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_Entry",
    "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_CategoryEntry",
    "/Game/SoulslikeFramework/Data/PDA_CustomSettings",
    "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings",
]

def extract_all_properties(obj, name):
    """Extract all properties from an object using reflection."""
    data = {"_class": str(obj.get_class().get_name())}

    # Try to get properties via export_text which gives us raw property values
    try:
        export_text = obj.export_text()
        for line in export_text.split('\n'):
            line = line.strip()
            if '=' in line:
                key, _, value = line.partition('=')
                key = key.strip()
                value = value.strip()
                if key and value:
                    data[key] = value
    except Exception as e:
        pass

    # Try direct attribute access for known properties
    # Include both snake_case (C++) and PascalCase (Blueprint) versions
    KNOWN_PROPS = [
        # Snake case (C++ properties)
        'unhovered_color', 'hovered_color', 'selected_color', 'deselected_color',
        'setting_tag', 'setting_name', 'setting_description', 'entry_type',
        'setting_category', 'switcher_index', 'custom_settings_asset',
        'categories_to_hide', 'quit_to_desktop',
        # PascalCase (Blueprint properties)
        'UnhoveredColor', 'HoveredColor', 'SelectedColor', 'DeselectedColor',
        'SettingTag', 'SettingName', 'SettingDescription', 'EntryType',
        'SettingCategory', 'SwitcherIndex', 'CustomSettingsAsset',
        'CategoriesToHide', 'QuitToDesktop', 'ButtonText', 'CurrentValue',
        'CategoryName', 'CategoryIcon', 'Selected?', 'Selected'
    ]

    for prop_name in KNOWN_PROPS:
        try:
            value = obj.get_editor_property(prop_name)
            if value is not None:
                # Handle LinearColor
                if hasattr(value, 'r') and hasattr(value, 'g') and hasattr(value, 'b'):
                    data[prop_name] = {"R": float(value.r), "G": float(value.g), "B": float(value.b), "A": float(value.a)}
                # Handle GameplayTag
                elif hasattr(value, 'tag_name'):
                    data[prop_name] = str(value.tag_name)
                elif isinstance(value, (int, float, bool)):
                    data[prop_name] = value
                else:
                    data[prop_name] = str(value)
        except Exception as e:
            pass

    return data

def main():
    results = {}

    for path in WIDGETS:
        name = path.split('/')[-1]
        unreal.log_warning(f"=== Extracting {name} ===")

        bp = unreal.load_asset(path)
        if not bp:
            unreal.log_warning(f"  FAILED to load")
            continue

        # Get the generated class and CDO
        gen_class = None
        if hasattr(bp, 'generated_class'):
            gen_class = bp.generated_class()
        if gen_class:
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                results[name] = extract_all_properties(cdo, name)
                unreal.log_warning(f"  Extracted {len(results[name])} properties")
            else:
                unreal.log_warning(f"  No CDO found")
        else:
            unreal.log_warning(f"  No generated_class")

    # Determine output file based on project
    project_name = str(unreal.Paths.get_project_file_path())
    if "bp_only" in project_name:
        output_file = OUTPUT_DIR + "settings_bponly.json"
    else:
        output_file = OUTPUT_DIR + "settings_current.json"

    # Save results
    os.makedirs(OUTPUT_DIR, exist_ok=True)
    with open(output_file, 'w') as f:
        json.dump(results, f, indent=2, default=str)

    unreal.log_warning(f"=== Saved to {output_file} ===")

main()
