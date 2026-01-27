import unreal
import os
import json

output_file = "C:/scripts/SLFConversion/migration_cache/settings_icons.json"
os.makedirs(os.path.dirname(output_file), exist_ok=True)

result = {}

# Load W_Settings widget blueprint
settings_path = "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings"
settings_bp = unreal.load_asset(settings_path)

if settings_bp:
    print(f"Loaded W_Settings: {settings_bp}")

    # Try to access the widget tree
    try:
        widget_tree = settings_bp.widget_tree
        if widget_tree:
            all_widgets = widget_tree.all_widgets
            print(f"Found {len(all_widgets)} widgets")

            for w in all_widgets:
                widget_name = w.get_name()
                widget_class = w.get_class().get_name()

                # Look for category entry widgets
                if "CategoryEntry" in widget_name:
                    print(f"\nWidget: {widget_name} ({widget_class})")

                    entry_data = {"name": widget_name, "class": widget_class}

                    # Try to get Icon property
                    try:
                        icon = w.get_editor_property("icon")
                        if icon:
                            icon_path = icon.get_path_name() if hasattr(icon, 'get_path_name') else str(icon)
                            print(f"  Icon: {icon_path}")
                            entry_data["icon"] = icon_path
                        else:
                            print(f"  Icon: None")
                            entry_data["icon"] = None
                    except Exception as e:
                        print(f"  Icon error: {e}")
                        entry_data["icon_error"] = str(e)

                    # Try to get Category property
                    try:
                        category = w.get_editor_property("category")
                        print(f"  Category: {category}")
                        entry_data["category"] = str(category)
                    except Exception as e:
                        pass

                    result[widget_name] = entry_data
    except Exception as e:
        print(f"Widget tree error: {e}")
else:
    print(f"Failed to load {settings_path}")

# Also check individual category entry widgets in the folder
category_entry_path = "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_CategoryEntry"
cat_bp = unreal.load_asset(category_entry_path)

if cat_bp:
    print(f"\nLoaded CategoryEntry template: {cat_bp}")
    gen_class = cat_bp.generated_class()
    if gen_class:
        cdo = unreal.get_default_object(gen_class)
        print(f"CDO: {cdo.get_name()}")

        # List all properties
        for prop_name in ['icon', 'category', 'category_icon', 'text', 'selected', 'index']:
            try:
                val = cdo.get_editor_property(prop_name)
                if val:
                    if hasattr(val, 'get_path_name'):
                        print(f"  {prop_name}: {val.get_path_name()}")
                    else:
                        print(f"  {prop_name}: {val}")
                else:
                    print(f"  {prop_name}: None")
            except Exception as e:
                pass  # Property doesn't exist

with open(output_file, "w") as f:
    json.dump(result, f, indent=2)

print(f"\nWritten to {output_file}")
