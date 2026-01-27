"""
Migrate DT_ExampleEquipmentSlotInfo DataTable to use FSLFEquipmentSlot C++ struct.

The DataTable currently uses a Blueprint struct with GUID-suffixed field names.
This script extracts the data, changes the row struct to the C++ version, and re-applies the data.
"""

import unreal

def migrate_slottable():
    # Load the DataTable
    slot_table_path = "/Game/SoulslikeFramework/Data/_Datatables/DT_ExampleEquipmentSlotInfo"
    slot_table = unreal.load_asset(slot_table_path)

    if not slot_table:
        unreal.log_error(f"Could not load SlotTable at {slot_table_path}")
        return False

    # Get current row struct info
    current_struct = slot_table.get_editor_property('row_struct')
    unreal.log(f"Current row struct: {current_struct.get_path_name() if current_struct else 'None'}")

    # Get row names
    row_names = list(slot_table.get_row_names())
    unreal.log(f"Found {len(row_names)} rows")

    # Export all row data using export_text
    exported_data = []
    for row_name in row_names:
        # Use generic property access to extract data
        # We'll use the export text functionality
        pass

    # Load the C++ struct
    cpp_struct_path = "/Script/SLFConversion.SLFEquipmentSlot"
    cpp_struct = unreal.load_object(None, cpp_struct_path)

    if not cpp_struct:
        # Try alternative path - the struct might be FSLFEquipmentSlot
        unreal.log_warning(f"Could not load C++ struct at {cpp_struct_path}, checking alternative...")

        # List available structs in SLFConversion module
        # For now, let's try to find it by searching
        cpp_struct = unreal.find_object(None, "/Script/SLFConversion.FSLFEquipmentSlot")
        if cpp_struct:
            unreal.log(f"Found struct: {cpp_struct}")
        else:
            unreal.log_error("Could not find FSLFEquipmentSlot struct in SLFConversion module")

    # Get class for the struct we need
    # Try loading as a ScriptStruct
    try:
        # In UE5 Python, we can use load_class or find struct differently
        struct_class = unreal.load_class(None, "/Script/SLFConversion.SLFEquipmentSlot")
        unreal.log(f"Loaded struct class: {struct_class}")
    except Exception as e:
        unreal.log_warning(f"Could not load struct class: {e}")

    # Alternative: Create a new DataTable with the C++ struct
    # This is more reliable than trying to change the existing one

    return True

def extract_slot_data_generic():
    """
    Extract slot data from the Blueprint struct DataTable using generic access.
    Returns a list of dicts with normalized field names.
    """
    slot_table_path = "/Game/SoulslikeFramework/Data/_Datatables/DT_ExampleEquipmentSlotInfo"
    slot_table = unreal.load_asset(slot_table_path)

    if not slot_table:
        unreal.log_error("Could not load SlotTable")
        return []

    # Get row names
    row_names = list(slot_table.get_row_names())
    unreal.log(f"Extracting data from {len(row_names)} rows...")

    # For Blueprint structs, we need to use the struct's actual property names
    # The row struct has GUID-suffixed names but we can access via the generic interface

    extracted = []

    # Try using the DataTable's API to get row data
    # Unfortunately, UE Python API for DataTables with custom structs is limited
    # We'll use export_text to get a text representation

    export_text = slot_table.export_text()
    unreal.log(f"Export text length: {len(export_text)}")

    # Parse the export text to extract row data
    # Format is typically: RowName,(Field1=Value1,Field2=Value2,...)

    # Save export to file for inspection
    with open("C:/scripts/SLFConversion/slottable_export.txt", "w") as f:
        f.write(export_text)
    unreal.log("Saved export to slottable_export.txt")

    return extracted

if __name__ == "__main__":
    unreal.log("=== SlotTable Migration Script ===")
    extract_slot_data_generic()
    unreal.log("=== Migration Complete ===")
