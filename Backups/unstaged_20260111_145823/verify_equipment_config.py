"""
Verify W_Equipment configuration.
"""

import unreal

def verify_config():
    # Load W_Equipment widget Blueprint
    equipment_widget_path = "/Game/SoulslikeFramework/Widgets/Equipment/W_Equipment"
    equipment_widget = unreal.load_asset(equipment_widget_path)

    if not equipment_widget:
        unreal.log_error(f"Could not load W_Equipment at {equipment_widget_path}")
        return

    # Get the CDO
    cdo = unreal.get_default_object(equipment_widget.generated_class())

    if not cdo:
        unreal.log_error("Could not get CDO for W_Equipment")
        return

    # Check EquipmentSlotClass
    try:
        slot_class = cdo.get_editor_property('equipment_slot_class')
        if slot_class:
            unreal.log(f"EquipmentSlotClass is SET: {slot_class.get_name()}")
        else:
            unreal.log_warning("EquipmentSlotClass is NOT SET")
    except Exception as e:
        unreal.log_error(f"Error checking EquipmentSlotClass: {e}")

    # Check InventorySlotClass
    try:
        inv_slot_class = cdo.get_editor_property('inventory_slot_class')
        if inv_slot_class:
            unreal.log(f"InventorySlotClass is SET: {inv_slot_class.get_name()}")
        else:
            unreal.log_warning("InventorySlotClass is NOT SET")
    except Exception as e:
        unreal.log_error(f"Error checking InventorySlotClass: {e}")

    # Check SlotTable on equipment manager
    slot_table_path = "/Game/SoulslikeFramework/Data/_Datatables/DT_ExampleEquipmentSlotInfo"
    slot_table = unreal.load_asset(slot_table_path)
    if slot_table:
        rows = slot_table.get_row_names()
        unreal.log(f"SlotTable has {len(rows)} rows: {rows}")
    else:
        unreal.log_warning(f"SlotTable not found at {slot_table_path}")

if __name__ == "__main__":
    verify_config()
