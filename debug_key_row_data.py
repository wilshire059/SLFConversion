"""
Debug the actual row data in DT_KeyMappingCorrelation to see property values.
"""

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/key_row_data_debug.txt"
OUTPUT_LINES = []

def log(msg):
    OUTPUT_LINES.append(str(msg))
    unreal.log_warning(str(msg))

def debug_row_data():
    log("=" * 60)
    log("DEBUGGING KEY ROW DATA")
    log("=" * 60)

    data_table_path = "/Game/SoulslikeFramework/Data/_Datatables/DT_KeyMappingCorrelation.DT_KeyMappingCorrelation"
    data_table = unreal.EditorAssetLibrary.load_asset(data_table_path)

    if not data_table:
        log("ERROR: Could not load data table")
        return

    # Get the row struct type
    row_struct = data_table.get_editor_property('row_struct')
    log(f"Row struct path: {row_struct.get_path_name() if row_struct else 'None'}")

    log("")
    log("First 10 row names:")

    row_names = unreal.DataTableFunctionLibrary.get_data_table_row_names(data_table)
    for i, row_name in enumerate(row_names[:10]):
        log(f"  Row {i}: '{row_name}'")

    # Try to export data table to text to see actual row data
    log("")
    log("=" * 60)
    log("EXPORTING DATA TABLE TO TEXT")
    log("=" * 60)

    try:
        # Export the whole data table using export_text
        export_text = data_table.export_text()
        log(f"Export text length: {len(export_text)}")

        # Write full export to a separate file
        with open("C:/scripts/SLFConversion/key_table_full_export.txt", 'w', encoding='utf-8') as f:
            f.write(export_text)
        log("Full export written to key_table_full_export.txt")

        # Show first 3000 chars
        log("")
        log("First 3000 chars of export:")
        log(export_text[:3000])
    except Exception as e:
        log(f"Error exporting: {e}")
        import traceback
        log(traceback.format_exc())

if __name__ == "__main__":
    debug_row_data()

    with open(OUTPUT_FILE, 'w', encoding='utf-8') as f:
        f.write('\n'.join(OUTPUT_LINES))
    unreal.log_warning(f"Output written to {OUTPUT_FILE}")
