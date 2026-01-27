# check_stat_table.py
# Check what stats are in DT_ExampleStatTable
import unreal

def check_stat_table():
    """Check what stats are in the stat DataTable."""

    output_file = "C:/scripts/SLFConversion/migration_cache/stat_table_check.txt"

    table_path = "/Game/SoulslikeFramework/Data/_Datatables/DT_ExampleStatTable"
    table = unreal.EditorAssetLibrary.load_asset(table_path)

    with open(output_file, 'w') as f:
        f.write("="*80 + "\n")
        f.write("DT_ExampleStatTable ANALYSIS\n")
        f.write("="*80 + "\n\n")

        if not table:
            f.write(f"FAILED to load: {table_path}\n")
            return

        f.write(f"Table: {table.get_name()}\n")
        f.write(f"Class: {table.get_class().get_name()}\n\n")

        # Get row names
        row_names = unreal.DataTableFunctionLibrary.get_data_table_row_names(table)
        f.write(f"Row count: {len(row_names)}\n\n")

        weight_found = False

        for row_name in row_names:
            f.write(f"[{row_name}]\n")

            # Check if it's the Weight stat
            if "Weight" in str(row_name):
                weight_found = True
                f.write("  *** WEIGHT STAT ROW ***\n")

        f.write("\n" + "="*80 + "\n")
        if weight_found:
            f.write("WEIGHT STAT: FOUND in DataTable\n")
        else:
            f.write("WEIGHT STAT: NOT FOUND in DataTable - This is the root cause!\n")
        f.write("="*80 + "\n")

    unreal.log(f"Output saved to: {output_file}")

if __name__ == "__main__":
    check_stat_table()
