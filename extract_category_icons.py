import unreal
import os
import json

output_file = "C:/scripts/SLFConversion/migration_cache/category_icons.json"
os.makedirs(os.path.dirname(output_file), exist_ok=True)

result = {}

# Load W_Settings widget blueprint
settings_path = "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings"
settings_bp = unreal.load_asset(settings_path)

if settings_bp:
    unreal.log(f"Loaded W_Settings: {settings_bp}")

    # Get the generated class and CDO
    gen_class = settings_bp.generated_class()
    if gen_class:
        cdo = unreal.get_default_object(gen_class)

        # Look for the category entry widgets as variables
        for var_name in ['w_settings_category_entry_display', 'w_settings_category_entry_camera',
                         'w_settings_category_entry_gameplay', 'w_settings_category_entry_keybinds',
                         'w_settings_category_entry_sounds', 'w_settings_category_entry_quit']:
            try:
                entry = cdo.get_editor_property(var_name)
                if entry:
                    unreal.log(f"\nFound {var_name}: {entry}")
                    result[var_name] = {"found": True}

                    # Try to get the Icon property from this entry widget
                    try:
                        icon = entry.get_editor_property('icon')
                        if icon:
                            icon_path = str(icon)
                            unreal.log(f"  Icon: {icon_path}")
                            result[var_name]["icon"] = icon_path
                        else:
                            unreal.log(f"  Icon: None")
                            result[var_name]["icon"] = None
                    except Exception as e:
                        unreal.log(f"  Icon error: {e}")
                        result[var_name]["icon_error"] = str(e)
            except Exception as e:
                unreal.log(f"{var_name}: Not found ({e})")
                result[var_name] = {"found": False, "error": str(e)}

# Also try to export the widget tree
try:
    text = unreal.EditorAssetLibrary.export_text(settings_path)
    if text:
        # Find Icon references in the export
        lines = text.split('\n')
        icon_lines = []
        for i, line in enumerate(lines):
            if 'Icon' in line or 'T_' in line:
                icon_lines.append(f"Line {i}: {line.strip()[:200]}")

        # Save some context
        with open("C:/scripts/SLFConversion/migration_cache/w_settings_export_snippet.txt", "w", encoding='utf-8') as f:
            f.write("\n".join(icon_lines[:100]))
        unreal.log(f"Found {len(icon_lines)} icon-related lines")
except Exception as e:
    unreal.log(f"Export error: {e}")

# Try to directly load each texture to verify they exist
textures = {
    "T_Computer": "/Game/SoulslikeFramework/Widgets/_Textures/T_Computer",
    "T_Camera": "/Game/SoulslikeFramework/Widgets/_Textures/T_Camera",
    "T_Controller": "/Game/SoulslikeFramework/Widgets/_Textures/T_Controller",
    "T_Keybinds": "/Game/SoulslikeFramework/Widgets/_Textures/T_Keybinds",
    "T_Volume": "/Game/SoulslikeFramework/Widgets/_Textures/T_Volume",
    "T_Quit": "/Game/SoulslikeFramework/Widgets/_Textures/T_Quit",
    "T_Gesture": "/Game/SoulslikeFramework/Widgets/_Textures/T_Gesture"
}

unreal.log("\n=== Texture Verification ===")
texture_results = {}
for name, path in textures.items():
    tex = unreal.load_asset(path)
    if tex:
        unreal.log(f"  {name}: FOUND ({tex.get_class().get_name()})")
        texture_results[name] = {"found": True, "class": tex.get_class().get_name()}
    else:
        unreal.log(f"  {name}: NOT FOUND")
        texture_results[name] = {"found": False}

result["textures"] = texture_results

with open(output_file, "w") as f:
    json.dump(result, f, indent=2)

unreal.log(f"\nWritten to {output_file}")
