"""
Export DayNight asset as raw text to find the time values
"""
import unreal
import subprocess
import os

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/daynight_raw_export.txt"

def main():
    # Try to export using unreal commandlet
    asset_path = "/Game/SoulslikeFramework/Data/DayNight/DA_ExampleDayNightInfo"

    asset = unreal.EditorAssetLibrary.load_asset(asset_path)
    if not asset:
        unreal.log_warning(f"Could not load: {asset_path}")
        return

    unreal.log_warning(f"Loaded: {asset.get_name()}")

    # Try to iterate struct array and access raw data
    try:
        entries_prop = asset.get_editor_property('entries')
        unreal.log_warning(f"Entries type: {type(entries_prop)}")
        unreal.log_warning(f"Entries count: {len(entries_prop) if entries_prop else 0}")

        if entries_prop:
            for i, entry in enumerate(entries_prop):
                unreal.log_warning(f"Entry {i}:")
                unreal.log_warning(f"  Type: {type(entry)}")
                unreal.log_warning(f"  Dir: {[x for x in dir(entry) if not x.startswith('_')]}")

                # Try all potential property names
                possible_props = ['time', 'Time', 'display_name', 'DisplayName',
                                  'name', 'Name', 'entry_name', 'EntryName',
                                  'hour', 'Hour', 'value', 'Value']
                for prop in possible_props:
                    try:
                        val = getattr(entry, prop, None)
                        if val is not None:
                            unreal.log_warning(f"  {prop}: {val}")
                    except:
                        pass

    except Exception as e:
        unreal.log_warning(f"Error: {e}")

    unreal.log_warning(f"\nDone")

if __name__ == "__main__":
    main()
