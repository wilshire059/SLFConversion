"""
Extract DayNight entries from bp_only with all struct details
Uses AssetRegistryHelpers to get property values
"""
import unreal
import json

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/daynight_entries_detailed.txt"

def main():
    with open(OUTPUT_FILE, "w", encoding="utf-8") as f:
        f.write("")

    def log(msg):
        unreal.log_warning(msg)
        with open(OUTPUT_FILE, "a", encoding="utf-8") as f:
            f.write(msg + "\n")

    log("=" * 70)
    log("EXTRACTING DAYNIGHT ENTRIES FROM BP_ONLY")
    log("=" * 70)

    # Load the data asset
    asset_path = "/Game/SoulslikeFramework/Data/DayNight/DA_ExampleDayNightInfo"
    asset = unreal.EditorAssetLibrary.load_asset(asset_path)

    if not asset:
        log(f"Could not load: {asset_path}")
        return

    log(f"Loaded: {asset.get_name()}")
    log(f"Class: {asset.get_class().get_name()}")

    # Try to get Entries using different methods
    try:
        entries = asset.get_editor_property('entries')
        log(f"\nEntries count: {len(entries) if entries else 0}")

        if entries:
            for i, entry in enumerate(entries):
                log(f"\n--- Entry {i} ---")
                log(f"  Type: {type(entry)}")

                # Get all attributes
                attrs = [a for a in dir(entry) if not a.startswith('_')]
                log(f"  Attributes: {attrs[:20]}")  # First 20

                # Try common property accessors
                for attr in attrs:
                    if attr in ['cast', 'get_class', 'get_editor_property', 'get_fname',
                                'get_full_name', 'get_name', 'get_outer', 'get_outermost',
                                'get_path_name', 'get_typed_outer', 'get_world', 'is_a',
                                'modify', 'rename', 'set_editor_property', 'static_class',
                                'static_struct', 'to_tuple']:
                        continue
                    try:
                        val = getattr(entry, attr)
                        if not callable(val):
                            log(f"  {attr}: {val}")
                    except Exception as e:
                        pass

                # Try get_editor_property with known Blueprint struct property names
                bp_props = [
                    'tag', 'Tag', 'tag_13_8de95b234226c6ca36cc289782937abd',
                    'name', 'Name', 'name_3_de5ddafa478f0da09d298fb5c3826044',
                    'from_time', 'FromTime', 'fromtime_9_e9b93b79414d557f282138ba47e1d055',
                    'to_time', 'ToTime', 'totime_10_e9595da4470d791d7d33af8354453aea',
                ]
                for prop in bp_props:
                    try:
                        val = entry.get_editor_property(prop)
                        log(f"  get_editor_property('{prop}'): {val}")
                    except:
                        pass

    except Exception as e:
        log(f"Error: {e}")
        import traceback
        log(traceback.format_exc())

    log(f"\n\nResults: {OUTPUT_FILE}")

if __name__ == "__main__":
    main()
