import unreal
import json
import os

output_file = "C:/scripts/SLFConversion/migration_cache/category_icons_check.json"
os.makedirs(os.path.dirname(output_file), exist_ok=True)

results = {}

# Load W_Settings widget blueprint
settings_path = "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings"
settings_bp = unreal.load_asset(settings_path)

if settings_bp:
    gen_class = settings_bp.generated_class()
    results["w_settings_class"] = str(gen_class) if gen_class else "None"

    if gen_class:
        # Get the CDO to check default subobjects
        cdo = unreal.get_default_object(gen_class)
        results["cdo"] = str(cdo)

        # Try to access the widget tree from the CDO
        # Widget trees are typically stored in the WidgetTree property
        try:
            widget_tree = cdo.get_editor_property("widget_tree")
            results["widget_tree"] = str(widget_tree)
        except:
            results["widget_tree"] = "Could not access"

# Check each CategoryEntry widget
category_entries = [
    "W_Settings_CategoryEntry_Display",
    "W_Settings_CategoryEntry_Camera",
    "W_Settings_CategoryEntry_Gameplay",
    "W_Settings_CategoryEntry_Keybinds",
    "W_Settings_CategoryEntry_Sounds",
    "W_Settings_CategoryEntry_Quit"
]

# Load the CategoryEntry blueprint to check its class and Icon property
entry_bp_path = "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_CategoryEntry"
entry_bp = unreal.load_asset(entry_bp_path)

if entry_bp:
    entry_class = entry_bp.generated_class()
    results["entry_class"] = str(entry_class) if entry_class else "None"

    if entry_class:
        entry_cdo = unreal.get_default_object(entry_class)
        results["entry_cdo"] = str(entry_cdo)

        # Check if Icon property exists on the CDO
        try:
            icon = entry_cdo.get_editor_property("icon")
            if icon:
                path = icon.get_path_name() if hasattr(icon, 'get_path_name') else str(icon)
                results["entry_cdo_icon"] = path
            else:
                results["entry_cdo_icon"] = "None"
        except Exception as e:
            results["entry_cdo_icon_error"] = str(e)

        # Check parent class
        parent = entry_class.get_super_class()
        results["entry_parent_class"] = str(parent) if parent else "None"

# Check actual textures exist
texture_paths = {
    "T_Computer": "/Game/SoulslikeFramework/Widgets/_Textures/T_Computer",
    "T_Camera": "/Game/SoulslikeFramework/Widgets/_Textures/T_Camera",
    "T_Controller": "/Game/SoulslikeFramework/Widgets/_Textures/T_Controller",
    "T_Gesture": "/Game/SoulslikeFramework/Widgets/_Textures/T_Gesture",
    "T_Keybinds": "/Game/SoulslikeFramework/Widgets/_Textures/T_Keybinds",
    "T_Volume": "/Game/SoulslikeFramework/Widgets/_Textures/T_Volume",
    "T_Quit": "/Game/SoulslikeFramework/Widgets/_Textures/T_Quit"
}

texture_check = {}
for name, path in texture_paths.items():
    tex = unreal.load_asset(path)
    if tex:
        texture_check[name] = {
            "found": True,
            "class": tex.get_class().get_name(),
            "size_x": tex.blueprint_get_size_x() if hasattr(tex, 'blueprint_get_size_x') else "unknown",
            "size_y": tex.blueprint_get_size_y() if hasattr(tex, 'blueprint_get_size_y') else "unknown"
        }
    else:
        texture_check[name] = {"found": False}

results["textures"] = texture_check

with open(output_file, "w") as f:
    json.dump(results, f, indent=2)

print(f"Results saved to {output_file}")
print(json.dumps(results, indent=2))
