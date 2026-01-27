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
    unreal.log(f"Loaded W_Settings: {settings_bp}")
    unreal.log(f"Class: {settings_bp.get_class().get_name()}")

    # Get generated class and CDO
    gen_class = settings_bp.generated_class()
    if gen_class:
        cdo = unreal.get_default_object(gen_class)
        unreal.log(f"CDO: {cdo}")

        # List all properties
        unreal.log("CDO Properties:")
        for prop_name in dir(cdo):
            if not prop_name.startswith('_'):
                try:
                    val = cdo.get_editor_property(prop_name)
                    if val is not None and "category" in prop_name.lower():
                        unreal.log(f"  {prop_name}: {val}")
                except:
                    pass

    # Try to get the blueprint's WidgetTree
    try:
        # Get the WidgetBlueprintGeneratedClass
        bp_class = settings_bp.get_class()
        unreal.log(f"BP Class: {bp_class.get_name()}")

        # Try accessing via Blueprint
        wt = getattr(settings_bp, 'widget_tree', None)
        if wt:
            unreal.log(f"WidgetTree: {wt}")
            all_widgets = wt.all_widgets if hasattr(wt, 'all_widgets') else []
            unreal.log(f"Found {len(all_widgets)} widgets")
            for w in all_widgets:
                unreal.log(f"  Widget: {w.get_name()}")
        else:
            unreal.log("No widget_tree attribute")
    except Exception as e:
        unreal.log(f"Error accessing widget tree: {e}")

# Check W_Settings_CategoryEntry template
cat_path = "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_CategoryEntry"
cat_bp = unreal.load_asset(cat_path)

if cat_bp:
    unreal.log(f"\nLoaded CategoryEntry: {cat_bp}")

    gen_class = cat_bp.generated_class()
    if gen_class:
        cdo = unreal.get_default_object(gen_class)

        # Check icon property
        unreal.log("Checking CDO properties:")
        for prop in ['icon', 'Icon', 'category', 'Category', 'index', 'Index', 'text', 'Text']:
            try:
                val = cdo.get_editor_property(prop)
                if val is not None:
                    if hasattr(val, 'get_path_name'):
                        unreal.log(f"  {prop}: {val.get_path_name()}")
                    else:
                        unreal.log(f"  {prop}: {val}")
                else:
                    unreal.log(f"  {prop}: None")
            except:
                pass

# Now use export_text to get detailed info
def get_export_text(asset_path):
    asset = unreal.load_asset(asset_path)
    if asset:
        try:
            text = unreal.EditorAssetLibrary.export_text(asset_path)
            return text
        except:
            return None
    return None

# Export W_Settings
unreal.log("\nExporting W_Settings...")
text = get_export_text(settings_path)
if text:
    # Save to file for analysis
    with open("C:/scripts/SLFConversion/migration_cache/w_settings_export.txt", "w", encoding='utf-8') as f:
        f.write(text)
    unreal.log(f"Exported W_Settings ({len(text)} chars)")

    # Look for Icon references
    lines = text.split('\n')
    for i, line in enumerate(lines):
        if 'Icon' in line and ('Texture' in line or 'T_' in line or 'SoftObjectPath' in line):
            unreal.log(f"  Line {i}: {line.strip()[:200]}")
            result[f"line_{i}"] = line.strip()[:500]

with open(output_file, "w") as f:
    json.dump(result, f, indent=2)

unreal.log(f"\nWritten to {output_file}")
