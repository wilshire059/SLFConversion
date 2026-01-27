import unreal

# Load the stat table
stat_table = unreal.load_asset("/Game/SoulslikeFramework/Data/_Datatables/DT_ExampleStatTable")
if stat_table:
    print(f"DataTable loaded: {stat_table.get_name()}")
    
    # Try to get row struct name
    try:
        row_struct = stat_table.get_editor_property('row_struct')
        if row_struct:
            print(f"Row struct name: {row_struct.get_name()}")
            print(f"Row struct path: {row_struct.get_path_name()}")
        else:
            print("Row struct is None")
    except Exception as e:
        print(f"Error getting row struct: {e}")
        
    # Get row names
    row_names = stat_table.get_row_names()
    print(f"Row count: {len(row_names)}")
    for name in row_names[:5]:
        print(f"  Row: {name}")
else:
    print("Failed to load DataTable")
