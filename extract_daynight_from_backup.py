"""
Extract DayNight entries from bp_only to see correct time values
"""
import unreal
import json

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/daynight_entries_bponly.json"

def main():
    # Load the data asset
    asset_path = "/Game/SoulslikeFramework/Data/DayNight/DA_ExampleDayNightInfo"

    asset = unreal.EditorAssetLibrary.load_asset(asset_path)
    if not asset:
        unreal.log_warning(f"Could not load: {asset_path}")
        return

    unreal.log_warning(f"Loaded: {asset.get_name()}")
    unreal.log_warning(f"Class: {asset.get_class().get_name()}")

    entries = []

    # Try to get Entries property
    try:
        entries_prop = asset.get_editor_property('entries')
        unreal.log_warning(f"Entries count: {len(entries_prop) if entries_prop else 0}")

        if entries_prop:
            for i, entry in enumerate(entries_prop):
                entry_data = {}

                # Try to get struct properties
                for prop_name in ['display_name', 'DisplayName', 'time', 'Time', 'icon', 'Icon',
                                  'start_time', 'StartTime', 'end_time', 'EndTime']:
                    try:
                        val = entry.get_editor_property(prop_name)
                        if val is not None:
                            entry_data[prop_name] = str(val)
                    except:
                        pass

                unreal.log_warning(f"  Entry {i}: {entry_data}")
                entries.append(entry_data)
    except Exception as e:
        unreal.log_warning(f"Error getting entries: {e}")

    # Also try to export the full asset as text
    try:
        export_text = unreal.EditorAssetLibrary.export_text(asset_path)
        if export_text:
            unreal.log_warning(f"\nExport text length: {len(export_text)}")
            # Write export text for analysis
            with open(OUTPUT_FILE.replace('.json', '_export.txt'), 'w', encoding='utf-8') as f:
                f.write(export_text)
    except Exception as e:
        unreal.log_warning(f"Export text error: {e}")

    # Save entries
    with open(OUTPUT_FILE, 'w', encoding='utf-8') as f:
        json.dump(entries, f, indent=2)

    unreal.log_warning(f"\nSaved to: {OUTPUT_FILE}")

if __name__ == "__main__":
    main()
