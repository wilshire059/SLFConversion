"""
Compare Rest System assets between bp_only and SLFConversion
Checks: B_SkyManager, PDA_DayNight, DA_ExampleDayNightInfo, W_TimePass, W_RestMenu
"""
import unreal
import json
import os

OUTPUT_DIR = "C:/scripts/SLFConversion/migration_cache/rest_system_comparison"
os.makedirs(OUTPUT_DIR, exist_ok=True)

def export_actor_details(actor_path, output_name):
    """Export actor SCS components, variables, and CDO"""
    result = {
        "path": actor_path,
        "exists": False,
        "class": None,
        "parent_class": None,
        "scs_components": [],
        "variables": {},
        "cdo_properties": {}
    }

    bp = unreal.EditorAssetLibrary.load_asset(actor_path)
    if not bp:
        unreal.log_warning(f"  Could not load: {actor_path}")
        return result

    result["exists"] = True
    result["class"] = type(bp).__name__

    # Get generated class
    gen_class = None
    if hasattr(bp, 'generated_class'):
        gen_class = bp.generated_class()

    if gen_class:
        try:
            super_class = gen_class.get_super_class()
            result["parent_class"] = super_class.get_name() if super_class else "None"
        except:
            try:
                result["parent_class"] = str(gen_class.get_class().get_name())
            except:
                result["parent_class"] = "Unknown"

        # Get CDO
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            # Try to get common properties
            for prop_name in ['Time', 'CurrentTimeOfDay', 'TimeDilation', 'TimeInfoAsset',
                              'DayLengthMinutes', 'bTimePaused', 'SunriseTime', 'SunsetTime']:
                try:
                    val = cdo.get_editor_property(prop_name.lower().replace('_', ''))
                except:
                    try:
                        val = cdo.get_editor_property(prop_name)
                    except:
                        val = "N/A"
                if val != "N/A":
                    if hasattr(val, 'get_name'):
                        val = val.get_name()
                    elif hasattr(val, 'get_path_name'):
                        val = val.get_path_name()
                    result["cdo_properties"][prop_name] = str(val)

    # Get SCS components via export
    try:
        export_text = unreal.EditorAssetLibrary.export_text(actor_path)
        # Parse for component names
        import re
        scs_matches = re.findall(r'Begin Object Class=(\w+) Name="(\w+)"', export_text)
        for class_name, comp_name in scs_matches:
            result["scs_components"].append({"name": comp_name, "class": class_name})
    except Exception as e:
        result["scs_error"] = str(e)

    return result

def export_data_asset_details(asset_path, output_name):
    """Export data asset properties"""
    result = {
        "path": asset_path,
        "exists": False,
        "class": None,
        "parent_class": None,
        "properties": {}
    }

    asset = unreal.EditorAssetLibrary.load_asset(asset_path)
    if not asset:
        unreal.log_warning(f"  Could not load: {asset_path}")
        return result

    result["exists"] = True
    result["class"] = type(asset).__name__

    # Check if it's a Blueprint or native asset
    if hasattr(asset, 'generated_class'):
        gen_class = asset.generated_class()
        if gen_class:
            try:
                super_class = gen_class.get_super_class()
                result["parent_class"] = super_class.get_name() if super_class else "None"
            except:
                result["parent_class"] = "Unknown"
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                # Try entries property for PDA_DayNight
                try:
                    entries = cdo.get_editor_property('entries')
                    if entries:
                        result["properties"]["entries_count"] = len(entries)
                        entry_list = []
                        for i, entry in enumerate(entries):
                            entry_data = {}
                            try:
                                entry_data["name"] = str(entry.get_editor_property('name'))
                            except:
                                pass
                            try:
                                entry_data["from_time"] = float(entry.get_editor_property('from_time'))
                            except:
                                pass
                            try:
                                entry_data["to_time"] = float(entry.get_editor_property('to_time'))
                            except:
                                pass
                            entry_list.append(entry_data)
                        result["properties"]["entries"] = entry_list
                except Exception as e:
                    result["properties"]["entries_error"] = str(e)
    else:
        # Native data asset
        result["parent_class"] = asset.get_class().get_name()
        # Try to get entries
        try:
            entries = asset.get_editor_property('entries')
            if entries:
                result["properties"]["entries_count"] = len(entries)
                entry_list = []
                for entry in entries:
                    entry_data = {}
                    try:
                        entry_data["name"] = str(entry.get_editor_property('name'))
                    except:
                        try:
                            # Try Name with capital
                            entry_data["name"] = str(entry.name)
                        except:
                            pass
                    try:
                        entry_data["from_time"] = float(entry.get_editor_property('from_time'))
                    except:
                        try:
                            entry_data["from_time"] = float(entry.from_time)
                        except:
                            pass
                    try:
                        entry_data["to_time"] = float(entry.get_editor_property('to_time'))
                    except:
                        try:
                            entry_data["to_time"] = float(entry.to_time)
                        except:
                            pass
                    entry_list.append(entry_data)
                result["properties"]["entries"] = entry_list
        except Exception as e:
            result["properties"]["entries_error"] = str(e)

    return result

def export_widget_details(widget_path, output_name):
    """Export widget hierarchy and properties"""
    result = {
        "path": widget_path,
        "exists": False,
        "class": None,
        "parent_class": None,
        "widget_tree": [],
        "animations": [],
        "variables": []
    }

    widget = unreal.EditorAssetLibrary.load_asset(widget_path)
    if not widget:
        unreal.log_warning(f"  Could not load: {widget_path}")
        return result

    result["exists"] = True
    result["class"] = type(widget).__name__

    if hasattr(widget, 'generated_class'):
        gen_class = widget.generated_class()
        if gen_class:
            try:
                super_class = gen_class.get_super_class()
                result["parent_class"] = super_class.get_name() if super_class else "None"
            except:
                result["parent_class"] = "Unknown"

    # Get widget tree via export
    try:
        export_text = unreal.EditorAssetLibrary.export_text(widget_path)
        import re
        # Find widget components
        widget_matches = re.findall(r'Begin Object Class=(\w+) Name="(\w+)"', export_text)
        for class_name, widget_name in widget_matches:
            if 'Widget' in class_name or class_name in ['TextBlock', 'Image', 'Border', 'Button',
                                                          'VerticalBox', 'HorizontalBox', 'CanvasPanel',
                                                          'Overlay', 'WidgetSwitcher', 'SizeBox']:
                result["widget_tree"].append({"name": widget_name, "class": class_name})

        # Find animations
        anim_matches = re.findall(r'WidgetAnimation.*?Name="(\w+)"', export_text)
        result["animations"] = list(set(anim_matches))
    except Exception as e:
        result["export_error"] = str(e)

    return result

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("REST SYSTEM COMPARISON - Checking all related assets")
    unreal.log_warning("=" * 70)

    # Assets to check
    assets_to_check = {
        "B_SkyManager": {
            "type": "actor",
            "path": "/Game/SoulslikeFramework/Blueprints/Sky/B_SkyManager"
        },
        "PDA_DayNight": {
            "type": "data_asset",
            "path": "/Game/SoulslikeFramework/Data/PDA_DayNight"
        },
        "DA_ExampleDayNightInfo": {
            "type": "data_asset",
            "path": "/Game/SoulslikeFramework/Data/DayNight/DA_ExampleDayNightInfo"
        },
        "W_TimePass": {
            "type": "widget",
            "path": "/Game/SoulslikeFramework/Widgets/RestMenu/W_TimePass"
        },
        "W_RestMenu": {
            "type": "widget",
            "path": "/Game/SoulslikeFramework/Widgets/RestMenu/W_RestMenu"
        },
        "W_RestMenu_TimeEntry": {
            "type": "widget",
            "path": "/Game/SoulslikeFramework/Widgets/RestMenu/W_RestMenu_TimeEntry"
        }
    }

    results = {}

    for name, info in assets_to_check.items():
        unreal.log_warning(f"\n--- Checking: {name} ---")

        if info["type"] == "actor":
            results[name] = export_actor_details(info["path"], name)
        elif info["type"] == "data_asset":
            results[name] = export_data_asset_details(info["path"], name)
        elif info["type"] == "widget":
            results[name] = export_widget_details(info["path"], name)

        # Log key findings
        r = results[name]
        unreal.log_warning(f"  Exists: {r['exists']}")
        unreal.log_warning(f"  Class: {r.get('class', 'N/A')}")
        unreal.log_warning(f"  Parent: {r.get('parent_class', 'N/A')}")

        if 'scs_components' in r and r['scs_components']:
            unreal.log_warning(f"  SCS Components: {len(r['scs_components'])}")
            for comp in r['scs_components'][:10]:  # First 10
                unreal.log_warning(f"    - {comp['name']} ({comp['class']})")

        if 'properties' in r:
            if 'entries_count' in r['properties']:
                unreal.log_warning(f"  Entries Count: {r['properties']['entries_count']}")
            if 'entries' in r['properties']:
                for entry in r['properties']['entries']:
                    unreal.log_warning(f"    - {entry}")

        if 'cdo_properties' in r:
            for k, v in r['cdo_properties'].items():
                unreal.log_warning(f"  CDO.{k}: {v}")

        if 'animations' in r and r['animations']:
            unreal.log_warning(f"  Animations: {r['animations']}")

    # Save to file
    output_file = os.path.join(OUTPUT_DIR, "slfconversion_state.json")
    with open(output_file, 'w') as f:
        json.dump(results, f, indent=2, default=str)
    unreal.log_warning(f"\nSaved to: {output_file}")

main()
