import unreal

# Get the DataTable
stat_table_path = "/Game/SoulslikeFramework/Data/_Datatables/DT_ExampleStatTable"
stat_table = unreal.load_asset(stat_table_path)

if stat_table:
    # Get current row struct
    current_struct = stat_table.get_editor_property('row_struct')
    unreal.log(f"Current RowStruct: {current_struct.get_path_name() if current_struct else 'None'}")
    
    # Load the C++ struct
    cpp_struct = unreal.load_object(None, "/Script/SLFConversion.FSLFStatEntry")
    if cpp_struct:
        unreal.log(f"Found C++ struct: FSLFStatEntry")
        # Note: We cannot directly change the RowStruct without losing data
        # Instead, we need to work around this limitation
    else:
        unreal.log_error("Failed to load FSLFStatEntry C++ struct")
        
    # Print row names and count
    row_names = stat_table.get_row_names()
    unreal.log(f"DataTable has {len(row_names)} rows")
    for name in row_names[:5]:
        unreal.log(f"  Row: {name}")
else:
    unreal.log_error(f"Failed to load DataTable: {stat_table_path}")
