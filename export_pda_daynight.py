"""Export PDA_DayNight structure and DA_ExampleDayNightInfo entries from bp_only"""
import unreal
import json
import re

def export_daynight():
    output = {}

    # Check DA_ExampleDayNightInfo
    da_path = "/Game/SoulslikeFramework/Data/DayNight/DA_ExampleDayNightInfo"
    da = unreal.EditorAssetLibrary.load_asset(da_path)
    if da:
        unreal.log_warning(f"=== DA_ExampleDayNightInfo ===")
        unreal.log_warning(f"  Path: {da_path}")
        unreal.log_warning(f"  Class: {da.get_class().get_name()}")

        # Export asset registry
        try:
            registry = unreal.AssetRegistryHelpers.get_asset_registry()
            asset_data = registry.get_asset_by_object_path(da_path)
            if asset_data:
                unreal.log_warning(f"  Asset exists in registry")
        except:
            pass

        # Export using copy functionality - get property list
        try:
            # List all accessible properties
            props = []
            for attr in dir(da):
                if not attr.startswith('_') and not attr.startswith('get_') and not attr.startswith('set_'):
                    try:
                        val = getattr(da, attr)
                        if not callable(val):
                            props.append((attr, type(val).__name__))
                    except:
                        pass

            unreal.log_warning(f"  Accessible properties ({len(props)}):")
            for name, typ in props[:20]:
                unreal.log_warning(f"    {name}: {typ}")
        except Exception as e:
            unreal.log_warning(f"  Property list error: {e}")

        # Try getting Entries using Python unreal API
        try:
            # Try common property names
            for prop_name in ["entries", "Entries", "day_night_entries", "DayNightEntries", "time_entries", "TimeEntries"]:
                try:
                    entries = da.get_editor_property(prop_name)
                    if entries is not None:
                        unreal.log_warning(f"  Found property '{prop_name}' with {len(entries)} entries")
                        output["entries"] = []
                        for i, entry in enumerate(entries):
                            entry_data = {}
                            # Try to get entry properties
                            for entry_prop in ["name", "Name", "display_name", "DisplayName", "tag", "Tag",
                                                "from_time", "FromTime", "from", "From",
                                                "to_time", "ToTime", "to", "To"]:
                                try:
                                    val = entry.get_editor_property(entry_prop)
                                    if val is not None:
                                        if hasattr(val, 'to_export_text'):
                                            entry_data[entry_prop] = val.to_export_text()
                                        else:
                                            entry_data[entry_prop] = str(val)
                                except:
                                    pass
                            output["entries"].append(entry_data)
                            unreal.log_warning(f"    [{i}] {entry_data}")
                        break
                except:
                    pass
        except Exception as e:
            unreal.log_warning(f"  Entries error: {e}")

    # Also check PDA_DayNight base blueprint
    pda_path = "/Game/SoulslikeFramework/Data/PDA_DayNight"
    pda = unreal.EditorAssetLibrary.load_asset(pda_path)
    if pda:
        unreal.log_warning(f"\n=== PDA_DayNight (Base Blueprint) ===")
        unreal.log_warning(f"  Path: {pda_path}")
        unreal.log_warning(f"  Class: {pda.get_class().get_name()}")

    # Save output
    with open("C:/scripts/SLFConversion/migration_cache/pda_daynight_data.json", "w") as f:
        json.dump(output, f, indent=2)
    unreal.log_warning(f"\nSaved to migration_cache/pda_daynight_data.json")

export_daynight()
