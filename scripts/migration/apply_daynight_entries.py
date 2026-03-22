"""
Apply correct bp_only DayNight entries to DA_ExampleDayNightInfo
The Blueprint migration lost the entry data - this restores it from bp_only values
"""
import unreal

def main():
    # Load the data asset
    asset_path = "/Game/SoulslikeFramework/Data/DayNight/DA_ExampleDayNightInfo"
    asset = unreal.EditorAssetLibrary.load_asset(asset_path)

    if not asset:
        unreal.log_error(f"Could not load: {asset_path}")
        return

    unreal.log_warning(f"Loaded: {asset.get_name()}")
    unreal.log_warning(f"Class: {asset.get_class().get_name()}")

    # Check current entries
    try:
        current_entries = asset.get_editor_property('entries')
        unreal.log_warning(f"Current entries count: {len(current_entries) if current_entries else 0}")
        if current_entries:
            for i, entry in enumerate(current_entries):
                name = entry.get_editor_property('name')
                unreal.log_warning(f"  Entry {i}: {name}")
    except Exception as e:
        unreal.log_warning(f"Could not read current entries: {e}")

    # Create new entries matching bp_only EXACTLY
    # Data extracted from bp_only DA_ExampleDayNightInfo:
    # Entry 0: Early Morning (FromTime=9.0, ToTime=12.0)
    # Entry 1: Sunrise (FromTime=12.0, ToTime=14.0)
    # Entry 2: Afternoon (FromTime=14.0, ToTime=22.0)
    # Entry 3: Sunset (FromTime=22.0, ToTime=24.0)
    # Entry 4: Night (FromTime=0.0, ToTime=6.0)
    # Entry 5: Midnight (FromTime=6.0, ToTime=9.0)

    bp_only_entries = [
        {"name": "Early Morning", "from_time": 9.0, "to_time": 12.0},
        {"name": "Sunrise", "from_time": 12.0, "to_time": 14.0},
        {"name": "Afternoon", "from_time": 14.0, "to_time": 22.0},
        {"name": "Sunset", "from_time": 22.0, "to_time": 24.0},
        {"name": "Night", "from_time": 0.0, "to_time": 6.0},
        {"name": "Midnight", "from_time": 6.0, "to_time": 9.0},
    ]

    # Create new array of FSLFDayNightInfo structs
    new_entries = []
    for entry_data in bp_only_entries:
        # Create the struct
        entry = unreal.SLFDayNightInfo()
        entry.set_editor_property('name', entry_data["name"])
        entry.set_editor_property('from_time', entry_data["from_time"])
        entry.set_editor_property('to_time', entry_data["to_time"])
        # Tag is optional, leave as empty for now
        new_entries.append(entry)
        unreal.log_warning(f"Created entry: {entry_data['name']} (FromTime={entry_data['from_time']}, ToTime={entry_data['to_time']})")

    # Set the entries on the data asset
    try:
        asset.set_editor_property('entries', new_entries)
        unreal.log_warning(f"Set {len(new_entries)} entries on data asset")
    except Exception as e:
        unreal.log_error(f"Failed to set entries: {e}")
        return

    # Verify
    try:
        verify_entries = asset.get_editor_property('entries')
        unreal.log_warning(f"Verified entries count: {len(verify_entries)}")
        for i, entry in enumerate(verify_entries):
            name = entry.get_editor_property('name')
            from_time = entry.get_editor_property('from_time')
            to_time = entry.get_editor_property('to_time')
            unreal.log_warning(f"  Entry {i}: {name} (FromTime={from_time}, ToTime={to_time})")
    except Exception as e:
        unreal.log_warning(f"Verification failed: {e}")

    # Save
    if unreal.EditorAssetLibrary.save_asset(asset_path):
        unreal.log_warning(f"Saved: {asset_path}")
    else:
        unreal.log_error(f"Failed to save: {asset_path}")

    unreal.log_warning("Done!")

if __name__ == "__main__":
    main()
