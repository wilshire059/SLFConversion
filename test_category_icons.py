import unreal
import os
import json

output_file = "C:/scripts/SLFConversion/migration_cache/category_icon_test.json"
os.makedirs(os.path.dirname(output_file), exist_ok=True)

# Test loading the category entry textures
textures = {
    "T_Computer": "/Game/SoulslikeFramework/Widgets/_Textures/T_Computer.T_Computer",
    "T_Camera": "/Game/SoulslikeFramework/Widgets/_Textures/T_Camera.T_Camera",
    "T_Controller": "/Game/SoulslikeFramework/Widgets/_Textures/T_Controller.T_Controller",
    "T_Keybinds": "/Game/SoulslikeFramework/Widgets/_Textures/T_Keybinds.T_Keybinds",
    "T_Volume": "/Game/SoulslikeFramework/Widgets/_Textures/T_Volume.T_Volume",
    "T_Quit": "/Game/SoulslikeFramework/Widgets/_Textures/T_Quit.T_Quit",
    "T_Gesture": "/Game/SoulslikeFramework/Widgets/_Textures/T_Gesture.T_Gesture",
}

results = {}
print("=== Testing Category Icon Loading ===")

for name, path in textures.items():
    tex = unreal.load_asset(path)
    if tex:
        print(f"  {name}: SUCCESS - {tex.get_class().get_name()}")
        results[name] = {"status": "success", "path": path, "class": tex.get_class().get_name()}
    else:
        # Try without double name
        alt_path = path.rsplit(".", 1)[0]
        tex = unreal.load_asset(alt_path)
        if tex:
            print(f"  {name}: SUCCESS (alt path) - {tex.get_class().get_name()}")
            results[name] = {"status": "success_alt", "path": alt_path, "class": tex.get_class().get_name()}
        else:
            print(f"  {name}: FAILED - Both paths failed")
            results[name] = {"status": "failed", "tried_paths": [path, alt_path]}

# Now also check the W_Settings widget for category entry instances
print("\n=== Checking W_Settings Category Entries ===")
settings_path = "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings"
settings_bp = unreal.load_asset(settings_path)

if settings_bp:
    print(f"Loaded W_Settings: {settings_bp}")
    results["w_settings_loaded"] = True

    # Get generated class and CDO
    gen_class = settings_bp.generated_class()
    if gen_class:
        cdo = unreal.get_default_object(gen_class)

        # Try to find the category entry widget references
        for entry_name in ['w_settings_category_entry_display', 'w_settings_category_entry_camera',
                           'w_settings_category_entry_gameplay', 'w_settings_category_entry_keybinds',
                           'w_settings_category_entry_sounds', 'w_settings_category_entry_quit']:
            try:
                entry = getattr(cdo, entry_name, None)
                if entry:
                    print(f"  {entry_name}: Found = {entry}")
                    # Try to get Icon
                    try:
                        icon = entry.get_editor_property('icon')
                        icon_str = str(icon) if icon else "None"
                        print(f"    Icon: {icon_str}")
                    except Exception as e:
                        print(f"    Icon error: {e}")
            except Exception as e:
                pass
else:
    print(f"FAILED to load W_Settings")
    results["w_settings_loaded"] = False

# Write results to file
with open(output_file, "w") as f:
    json.dump(results, f, indent=2)

print(f"\n=== Results written to {output_file} ===")
