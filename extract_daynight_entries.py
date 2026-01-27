"""
Extract DayNight entries from bp_only PDA_DayNight
Run this on bp_only project to get the original data
"""
import unreal
import json

DA_PATH = "/Game/SoulslikeFramework/Data/DayNight/DA_ExampleDayNightInfo"
OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/daynight_entries.json"

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("EXTRACT DayNight Entries (from bp_only)")
    unreal.log_warning("=" * 70)

    da = unreal.EditorAssetLibrary.load_asset(DA_PATH)
    if not da:
        unreal.log_warning(f"ERROR: Could not load {DA_PATH}")
        return

    unreal.log_warning(f"Loaded: {da.get_name()}")

    entries_data = []

    # Try to get entries
    try:
        entries = da.get_editor_property('entries')
        unreal.log_warning(f"Found {len(entries)} entries")

        for i, entry in enumerate(entries):
            try:
                entry_dict = {}
                # Try various property names
                for prop_name in ['name', 'Name', 'display_name', 'DisplayName']:
                    try:
                        val = entry.get_editor_property(prop_name)
                        if val:
                            entry_dict['name'] = str(val)
                            break
                    except:
                        pass

                for prop_name in ['from_time', 'FromTime', 'from', 'start_time']:
                    try:
                        val = entry.get_editor_property(prop_name)
                        entry_dict['from_time'] = float(val)
                        break
                    except:
                        pass

                for prop_name in ['to_time', 'ToTime', 'to', 'end_time']:
                    try:
                        val = entry.get_editor_property(prop_name)
                        entry_dict['to_time'] = float(val)
                        break
                    except:
                        pass

                for prop_name in ['tag', 'Tag', 'gameplay_tag']:
                    try:
                        val = entry.get_editor_property(prop_name)
                        if val:
                            entry_dict['tag'] = str(val)
                            break
                    except:
                        pass

                entries_data.append(entry_dict)
                unreal.log_warning(f"  [{i}] {entry_dict}")
            except Exception as e:
                unreal.log_warning(f"  [{i}] Error: {e}")

    except Exception as e:
        unreal.log_warning(f"Error getting entries: {e}")

    # Save to JSON
    if entries_data:
        with open(OUTPUT_FILE, 'w') as f:
            json.dump(entries_data, f, indent=2)
        unreal.log_warning(f"\nSaved {len(entries_data)} entries to {OUTPUT_FILE}")
    else:
        unreal.log_warning("No entries to save")

    unreal.log_warning("=" * 70)

main()
