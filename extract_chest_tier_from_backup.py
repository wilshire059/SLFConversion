# extract_chest_tier_from_backup.py
# Extract DT_ExampleChestTier data from bp_only backup using export_text
# Run this on the bp_only project to get the original Blueprint struct data

import unreal
import re
import json

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/chest_tier_raw.json"
EXPORT_FILE = "C:/scripts/SLFConversion/migration_cache/chest_tier_export.txt"

def main():
    table_path = "/Game/SoulslikeFramework/Data/_Datatables/DT_ExampleChestTier"

    unreal.log_warning("=" * 70)
    unreal.log_warning("EXTRACTING DT_ExampleChestTier DATA")
    unreal.log_warning("=" * 70)

    # Load the DataTable
    table = unreal.EditorAssetLibrary.load_asset(table_path)
    if not table:
        unreal.log_error(f"Failed to load: {table_path}")
        return

    # Get row struct info
    row_struct = table.get_editor_property('row_struct')
    struct_path = row_struct.get_path_name() if row_struct else 'None'
    unreal.log_warning(f"Row Struct: {struct_path}")

    # Get row names
    row_names = list(table.get_row_names())
    unreal.log_warning(f"Row Count: {len(row_names)}")

    # Export text representation - this contains the actual data
    export_text = table.export_text()
    unreal.log_warning(f"Export text length: {len(export_text)}")

    # Save raw export for inspection
    with open(EXPORT_FILE, 'w') as f:
        f.write(export_text)
    unreal.log_warning(f"Saved raw export to: {EXPORT_FILE}")

    # Parse the export text to extract row data
    # Format is typically:
    # Begin Object Name="RowName"
    #    <Property>=<Value>
    # End Object

    rows_data = []

    # Split by row markers
    row_pattern = r'Begin Object[^}]+?Name="([^"]+)"[^}]+?End Object'

    # Actually the format is different - let's just save the raw export
    # and parse it manually or check the format first

    result = {
        "table_path": table_path,
        "row_struct": struct_path,
        "row_names": row_names,
        "export_text_preview": export_text[:5000]  # First 5000 chars
    }

    with open(OUTPUT_FILE, 'w') as f:
        json.dump(result, f, indent=2)
    unreal.log_warning(f"Saved result to: {OUTPUT_FILE}")

    # Print first 2000 chars of export for inspection
    unreal.log_warning("\n--- EXPORT PREVIEW ---")
    for line in export_text[:2000].split('\n'):
        unreal.log_warning(line)

    unreal.log_warning("=" * 70)

if __name__ == "__main__":
    main()
