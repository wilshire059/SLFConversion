"""
Populate DA_ExampleDayNightInfo with standard day/night entries
"""
import unreal

DA_PATH = "/Game/SoulslikeFramework/Data/DayNight/DA_ExampleDayNightInfo"

# Standard day/night entries from Soulslike framework
TIME_ENTRIES = [
    {"name": "Morning", "from_time": 6.0, "to_time": 10.0},
    {"name": "Noon", "from_time": 10.0, "to_time": 14.0},
    {"name": "Afternoon", "from_time": 14.0, "to_time": 18.0},
    {"name": "Evening", "from_time": 18.0, "to_time": 21.0},
    {"name": "Night", "from_time": 21.0, "to_time": 2.0},
    {"name": "Late Night", "from_time": 2.0, "to_time": 6.0},
]

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("POPULATE DayNight Entries")
    unreal.log_warning("=" * 70)

    da = unreal.EditorAssetLibrary.load_asset(DA_PATH)
    if not da:
        unreal.log_warning(f"ERROR: Could not load {DA_PATH}")
        return

    unreal.log_warning(f"Loaded: {da.get_name()}")

    # Get the struct type
    struct_type = unreal.find_object(None, "/Script/SLFConversion.SLFDayNightInfo")
    if not struct_type:
        # Try FSLFDayNightInfo
        struct_type = unreal.find_object(None, "/Script/SLFConversion.FSLFDayNightInfo")
    
    unreal.log_warning(f"Struct type: {struct_type}")

    # Create new entries array
    new_entries = []
    for entry_data in TIME_ENTRIES:
        try:
            # Create a new SLFDayNightInfo struct
            entry = unreal.SLFDayNightInfo()
            entry.set_editor_property('name', entry_data['name'])
            entry.set_editor_property('from_time', entry_data['from_time'])
            entry.set_editor_property('to_time', entry_data['to_time'])
            new_entries.append(entry)
            unreal.log_warning(f"  Created: {entry_data['name']} ({entry_data['from_time']} - {entry_data['to_time']})")
        except Exception as e:
            unreal.log_warning(f"  Error creating entry: {e}")
            # Try alternative approach
            try:
                entry = unreal.FSLFDayNightInfo()
                entry.set_editor_property('Name', entry_data['name'])
                entry.set_editor_property('FromTime', entry_data['from_time'])
                entry.set_editor_property('ToTime', entry_data['to_time'])
                new_entries.append(entry)
                unreal.log_warning(f"  Created (alt): {entry_data['name']}")
            except Exception as e2:
                unreal.log_warning(f"  Error with alt approach: {e2}")

    # Set entries on the data asset
    if new_entries:
        try:
            da.set_editor_property('entries', new_entries)
            unreal.log_warning(f"\nSet {len(new_entries)} entries on data asset")
        except Exception as e:
            unreal.log_warning(f"Error setting entries: {e}")

    # Save
    unreal.log_warning("\nSaving...")
    try:
        result = unreal.EditorAssetLibrary.save_asset(DA_PATH)
        unreal.log_warning(f"Save result: {result}")
    except Exception as e:
        unreal.log_warning(f"Save error: {e}")

    # Verify
    unreal.log_warning("\nVerifying...")
    da = unreal.EditorAssetLibrary.load_asset(DA_PATH)
    try:
        entries = da.get_editor_property('entries')
        unreal.log_warning(f"Entries count: {len(entries)}")
        for i, entry in enumerate(entries):
            try:
                name = entry.get_editor_property('name')
                from_t = entry.get_editor_property('from_time')
                to_t = entry.get_editor_property('to_time')
                unreal.log_warning(f"  [{i}] {name} ({from_t} - {to_t})")
            except Exception as e:
                unreal.log_warning(f"  [{i}] Error reading: {e}")
    except Exception as e:
        unreal.log_warning(f"Error reading entries: {e}")

    unreal.log_warning("=" * 70)

main()
