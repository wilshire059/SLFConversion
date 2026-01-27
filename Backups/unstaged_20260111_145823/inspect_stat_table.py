import unreal

output_file = "C:/scripts/SLFConversion/datatable_info.txt"
lines = []

lines.append("=" * 60)
lines.append("Inspecting DT_ExampleStatTable Row Structure")
lines.append("=" * 60)

# Load the stat table
stat_table = unreal.load_asset("/Game/SoulslikeFramework/Data/_Datatables/DT_ExampleStatTable")
if not stat_table:
    lines.append("ERROR: Failed to load DataTable")
else:
    lines.append(f"DataTable: {stat_table.get_name()}")
    
    # Try to get row struct property
    try:
        # Access row_struct property
        row_struct = stat_table.get_editor_property('row_struct')
        if row_struct:
            lines.append(f"Row Struct: {row_struct.get_name()}")
            lines.append(f"Row Struct Path: {row_struct.get_path_name()}")
            
            # Try to get struct properties
            # List all properties of the struct
            lines.append("\nStruct Properties:")
            for prop in row_struct.get_class().properties():
                lines.append(f"  - {prop.get_name()}")
        else:
            lines.append("Row struct is None")
    except Exception as e:
        lines.append(f"Row struct error: {e}")
    
    # Try alternative method - get metadata
    try:
        asset_data = unreal.EditorAssetLibrary.find_asset_data(stat_table.get_path_name())
        lines.append(f"\nAsset path: {stat_table.get_path_name()}")
        
        # Try to get tags
        all_tags = asset_data.get_tag_value("RowStructure") if asset_data else None
        lines.append(f"RowStructure tag: {all_tags}")
    except Exception as e:
        lines.append(f"Metadata error: {e}")
    
    # Get row names
    row_names = stat_table.get_row_names()
    lines.append(f"\nRow count: {len(row_names)}")
    
    # Try to find the original Blueprint struct
    try:
        bp_struct = unreal.load_asset("/Game/SoulslikeFramework/Structures/Stats/FStatInfo")
        if bp_struct:
            lines.append(f"\nBlueprint FStatInfo found: {bp_struct.get_name()}")
            lines.append(f"  Path: {bp_struct.get_path_name()}")
        else:
            lines.append("\nBlueprint FStatInfo NOT found at expected path")
    except Exception as e:
        lines.append(f"\nBlueprint struct lookup error: {e}")

lines.append("=" * 60)

# Write to file
with open(output_file, 'w') as f:
    f.write('\n'.join(lines))

unreal.log(f"Output written to: {output_file}")
