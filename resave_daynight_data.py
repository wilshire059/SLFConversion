"""
Resave DA_ExampleDayNightInfo to apply struct redirect
"""
import unreal

DA_PATH = "/Game/SoulslikeFramework/Data/DayNight/DA_ExampleDayNightInfo"

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("RESAVE DayNight Data Asset")
    unreal.log_warning("=" * 70)

    # Load the data asset
    da = unreal.EditorAssetLibrary.load_asset(DA_PATH)
    if not da:
        unreal.log_warning(f"ERROR: Could not load {DA_PATH}")
        return

    unreal.log_warning(f"Loaded: {da.get_name()}")
    unreal.log_warning(f"Class: {da.get_class().get_name()}")

    # Try to get entries
    try:
        entries = da.get_editor_property('entries')
        unreal.log_warning(f"Entries: {len(entries)}")
        for i, entry in enumerate(entries):
            try:
                name = entry.get_editor_property('name')
                from_time = entry.get_editor_property('from_time')
                to_time = entry.get_editor_property('to_time')
                unreal.log_warning(f"  [{i}] {name} ({from_time:.1f} - {to_time:.1f})")
            except Exception as e:
                unreal.log_warning(f"  [{i}] Error reading entry: {e}")
    except Exception as e:
        unreal.log_warning(f"Error getting entries: {e}")

    # Save the asset
    unreal.log_warning("\nSaving...")
    try:
        result = unreal.EditorAssetLibrary.save_asset(DA_PATH)
        unreal.log_warning(f"Save result: {result}")
    except Exception as e:
        unreal.log_warning(f"Save error: {e}")

    unreal.log_warning("=" * 70)

main()
