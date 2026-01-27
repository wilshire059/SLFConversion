"""
Check what row struct type the SlotTable DataTable uses.
"""

import unreal

slot_table_path = "/Game/SoulslikeFramework/Data/_Datatables/DT_ExampleEquipmentSlotInfo"
slot_table = unreal.load_asset(slot_table_path)

if slot_table:
    # Get the row struct
    row_struct = slot_table.get_editor_property('row_struct')
    unreal.log(f"SlotTable row_struct: {row_struct}")
    unreal.log(f"SlotTable row_struct path: {row_struct.get_path_name() if row_struct else 'None'}")

    # Get row names
    rows = slot_table.get_row_names()
    unreal.log(f"Row count: {len(rows)}")

    # Try to get first row data
    if rows:
        first_row = rows[0]
        unreal.log(f"First row name: {first_row}")
else:
    unreal.log_error(f"Could not load SlotTable at {slot_table_path}")
