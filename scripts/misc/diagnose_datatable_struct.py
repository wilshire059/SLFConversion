# diagnose_datatable_struct.py
# Check what row struct the DT_ExampleChestTier DataTable actually uses

import unreal

def main():
    table_path = "/Game/SoulslikeFramework/Data/_Datatables/DT_ExampleChestTier"

    unreal.log_warning("=" * 70)
    unreal.log_warning("DATATABLE ROW STRUCT DIAGNOSIS")
    unreal.log_warning("=" * 70)

    # Load the DataTable
    table = unreal.EditorAssetLibrary.load_asset(table_path)
    if not table:
        unreal.log_error(f"Failed to load: {table_path}")
        return

    unreal.log_warning(f"DataTable: {table.get_name()}")

    # Get the row struct
    row_struct = table.get_editor_property("row_struct")
    if row_struct:
        unreal.log_warning(f"Row Struct: {row_struct.get_name()}")
        unreal.log_warning(f"Row Struct Path: {row_struct.get_path_name()}")
    else:
        unreal.log_warning("Row Struct: <none>")

    # Get row names
    row_names = unreal.DataTableFunctionLibrary.get_data_table_row_names(table)
    unreal.log_warning(f"Row Count: {len(row_names)}")

    for i, row_name in enumerate(row_names[:5]):  # Show first 5
        unreal.log_warning(f"  Row {i}: {row_name}")

    # Try to read a row's data
    if row_names:
        unreal.log_warning(f"\nAttempting to read first row...")
        # Use generic row access
        try:
            # Get row data as string (export format)
            export_str = table.export_text()
            # Just show first 2000 chars
            unreal.log_warning(f"Export preview (first 2000 chars):\n{export_str[:2000]}")
        except Exception as e:
            unreal.log_warning(f"Could not export: {e}")

    unreal.log_warning("=" * 70)

if __name__ == "__main__":
    main()
