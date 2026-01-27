"""Get DayNight entries from DA_ExampleDayNightInfo instance"""
import unreal
import json

# Load the data asset instance
da_path = "/Game/SoulslikeFramework/Data/DayNight/DA_ExampleDayNightInfo"
da = unreal.EditorAssetLibrary.load_asset(da_path)
if da:
    unreal.log_warning(f"=== DA_ExampleDayNightInfo ===")
    unreal.log_warning(f"  Class: {da.get_class().get_name()}")

    # List all properties
    for prop in dir(da):
        if not prop.startswith('_'):
            try:
                val = getattr(da, prop)
                if not callable(val):
                    unreal.log_warning(f"  {prop}: {type(val).__name__}")
            except:
                pass

    # Try to get entries property
    try:
        entries = da.get_editor_property("entries")
        if entries:
            unreal.log_warning(f"\n  Entries ({len(entries)}):")
            entries_data = []
            for i, entry in enumerate(entries):
                entry_dict = {}
                try:
                    name = entry.get_editor_property("name")
                    tag = entry.get_editor_property("tag")
                    from_time = entry.get_editor_property("from_time")
                    to_time = entry.get_editor_property("to_time")
                    unreal.log_warning(f"    [{i}] {name} - Tag:{tag}, From:{from_time}, To:{to_time}")
                    entry_dict = {
                        "name": str(name),
                        "from_time": from_time,
                        "to_time": to_time
                    }
                    if tag:
                        entry_dict["tag"] = tag.get_editor_property("tag_name") if hasattr(tag, 'get_editor_property') else str(tag)
                except Exception as e:
                    unreal.log_warning(f"    [{i}] Error: {e}")
                entries_data.append(entry_dict)

            # Save to cache
            with open("C:/scripts/SLFConversion/migration_cache/daynight_entries.json", "w") as f:
                json.dump(entries_data, f, indent=2)
            unreal.log_warning(f"\n  Saved {len(entries_data)} entries to migration_cache/daynight_entries.json")
        else:
            unreal.log_warning(f"  Entries: None or empty")
    except Exception as e:
        unreal.log_warning(f"  Error getting entries: {e}")
else:
    unreal.log_warning(f"DA_ExampleDayNightInfo not found")
