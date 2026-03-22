# check_chest_tier_table.py
# Check the row structure of DT_ExampleChestTier

import unreal

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("CHECKING DT_ExampleChestTier")
    unreal.log_warning("=" * 70)

    # Load the data table
    dt_path = "/Game/SoulslikeFramework/Data/_Datatables/DT_ExampleChestTier"
    dt = unreal.load_asset(dt_path)
    if not dt:
        unreal.log_error(f"Could not load: {dt_path}")
        return

    unreal.log_warning(f"DataTable: {dt.get_name()}")
    unreal.log_warning(f"Class: {dt.get_class().get_name()}")

    # Get row struct
    try:
        row_struct = dt.get_editor_property("row_struct")
        if row_struct:
            unreal.log_warning(f"Row struct: {row_struct.get_name()}")
            unreal.log_warning(f"Row struct path: {row_struct.get_path_name()}")
    except Exception as e:
        unreal.log_warning(f"Error getting row struct: {e}")

    # Get row names
    try:
        row_names = dt.get_row_names()
        unreal.log_warning(f"\nRow count: {len(row_names)}")
        for name in row_names:
            unreal.log_warning(f"  Row: {name}")
    except Exception as e:
        unreal.log_warning(f"Error getting rows: {e}")

    unreal.log_warning("\n" + "=" * 70)

if __name__ == "__main__":
    main()
