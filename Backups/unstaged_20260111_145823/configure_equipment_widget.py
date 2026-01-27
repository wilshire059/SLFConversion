"""
Configure W_Equipment widget with required references for equipment slot creation.

This script sets:
1. EquipmentSlotClass on W_Equipment to the W_EquipmentSlot Blueprint
2. SlotTable on AC_EquipmentManager (if not already set)
"""

import unreal

def configure_equipment_widget():
    """Configure W_Equipment Blueprint with EquipmentSlotClass."""

    # Load W_Equipment widget Blueprint
    equipment_widget_path = "/Game/SoulslikeFramework/Widgets/Equipment/W_Equipment"
    equipment_widget = unreal.load_asset(equipment_widget_path)

    if not equipment_widget:
        print(f"ERROR: Could not load W_Equipment at {equipment_widget_path}")
        return False

    # Load W_EquipmentSlot class - use full class path with _C suffix
    equipment_slot_path = "/Game/SoulslikeFramework/Widgets/Equipment/W_EquipmentSlot.W_EquipmentSlot_C"
    equipment_slot_class = unreal.load_class(None, equipment_slot_path)

    if not equipment_slot_class:
        print(f"ERROR: Could not load W_EquipmentSlot class at {equipment_slot_path}")
        return False

    # Get the CDO (Class Default Object) of the widget
    cdo = unreal.get_default_object(equipment_widget.generated_class())

    if not cdo:
        print("ERROR: Could not get CDO for W_Equipment")
        return False

    # Set EquipmentSlotClass
    try:
        cdo.set_editor_property('equipment_slot_class', equipment_slot_class)
        print(f"OK: Set EquipmentSlotClass to {equipment_slot_class.get_name()}")
    except Exception as e:
        print(f"ERROR setting EquipmentSlotClass: {e}")
        return False

    # Also set InventorySlotClass if not set
    inventory_slot_path = "/Game/SoulslikeFramework/Widgets/Inventory/W_InventorySlot.W_InventorySlot_C"
    inventory_slot_class = unreal.load_class(None, inventory_slot_path)

    if inventory_slot_class:
        try:
            cdo.set_editor_property('inventory_slot_class', inventory_slot_class)
            print(f"OK: Set InventorySlotClass to {inventory_slot_class.get_name()}")
        except Exception as e:
            print(f"Warning: Could not set InventorySlotClass: {e}")

    # Save the asset
    unreal.EditorAssetLibrary.save_asset(equipment_widget_path)
    print(f"OK: Saved W_Equipment")

    return True

def configure_equipment_manager_slot_table():
    """Ensure AC_EquipmentManager has SlotTable configured."""

    # The SlotTable should be set on the character Blueprint's AC_EquipmentManager component
    # Let's check the data table exists
    slot_table_path = "/Game/SoulslikeFramework/Data/_Datatables/DT_ExampleEquipmentSlotInfo"
    slot_table = unreal.load_asset(slot_table_path)

    if not slot_table:
        print(f"ERROR: Could not load SlotTable at {slot_table_path}")
        return False

    print(f"OK: SlotTable exists at {slot_table_path}")
    print(f"    Row count: {len(slot_table.get_row_names())}")
    print(f"    Rows: {slot_table.get_row_names()}")

    # The SlotTable needs to be set on the AC_EquipmentManager component
    # This is typically done on the character Blueprint
    # For now, let's just verify the table exists and has data

    return True

def main():
    print("=" * 60)
    print("Configuring Equipment Widget System")
    print("=" * 60)

    success = True

    # Configure W_Equipment
    print("\n[1/2] Configuring W_Equipment...")
    if not configure_equipment_widget():
        success = False

    # Check SlotTable
    print("\n[2/2] Checking SlotTable...")
    if not configure_equipment_manager_slot_table():
        success = False

    print("\n" + "=" * 60)
    if success:
        print("Configuration complete!")
        print("\nNOTE: You may also need to ensure:")
        print("  - AC_EquipmentManager on your character has SlotTable set")
        print("  - SlotTable is: /Game/SoulslikeFramework/Data/_Datatables/DT_ExampleEquipmentSlotInfo")
    else:
        print("Configuration had errors - check output above")
    print("=" * 60)

    return success

if __name__ == "__main__":
    main()
