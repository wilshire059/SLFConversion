"""
Configure W_Equipment widget with required references for equipment slot creation.
Uses Blueprint editor subsystem for proper modification.
"""

import unreal

# Load W_Equipment widget Blueprint
equipment_widget_path = "/Game/SoulslikeFramework/Widgets/Equipment/W_Equipment"
equipment_widget = unreal.load_asset(equipment_widget_path)

if not equipment_widget:
    unreal.log_error(f"Could not load W_Equipment at {equipment_widget_path}")
else:
    unreal.log(f"Loaded W_Equipment: {equipment_widget}")

# Load W_EquipmentSlot Blueprint - just the Blueprint asset, not class
equipment_slot_bp_path = "/Game/SoulslikeFramework/Widgets/Equipment/W_EquipmentSlot"
equipment_slot_bp = unreal.load_asset(equipment_slot_bp_path)

if not equipment_slot_bp:
    unreal.log_error(f"Could not load W_EquipmentSlot BP at {equipment_slot_bp_path}")
else:
    unreal.log(f"Loaded W_EquipmentSlot BP: {equipment_slot_bp}")
    # Get the generated class from the Blueprint
    equipment_slot_class = equipment_slot_bp.generated_class()
    unreal.log(f"W_EquipmentSlot class: {equipment_slot_class}")

# Load W_InventorySlot Blueprint
inventory_slot_bp_path = "/Game/SoulslikeFramework/Widgets/Inventory/W_InventorySlot"
inventory_slot_bp = unreal.load_asset(inventory_slot_bp_path)

if inventory_slot_bp:
    inventory_slot_class = inventory_slot_bp.generated_class()
    unreal.log(f"W_InventorySlot class: {inventory_slot_class}")
else:
    inventory_slot_class = None
    unreal.log_warning(f"Could not load W_InventorySlot at {inventory_slot_bp_path}")

# Get the generated class from W_Equipment and its CDO
if equipment_widget:
    w_equipment_class = equipment_widget.generated_class()
    cdo = unreal.get_default_object(w_equipment_class)

    if cdo:
        unreal.log(f"Got CDO: {cdo}")

        # Set EquipmentSlotClass
        if equipment_slot_bp:
            try:
                cdo.set_editor_property('equipment_slot_class', equipment_slot_class)
                unreal.log("Set equipment_slot_class successfully")
            except Exception as e:
                unreal.log_error(f"Failed to set equipment_slot_class: {e}")

        # Set InventorySlotClass
        if inventory_slot_class:
            try:
                cdo.set_editor_property('inventory_slot_class', inventory_slot_class)
                unreal.log("Set inventory_slot_class successfully")
            except Exception as e:
                unreal.log_error(f"Failed to set inventory_slot_class: {e}")

        # Save the asset
        try:
            result = unreal.EditorAssetLibrary.save_asset(equipment_widget_path, only_if_is_dirty=False)
            unreal.log(f"Save result: {result}")
        except Exception as e:
            unreal.log_error(f"Failed to save: {e}")
    else:
        unreal.log_error("Could not get CDO")

# Check SlotTable
slot_table_path = "/Game/SoulslikeFramework/Data/_Datatables/DT_ExampleEquipmentSlotInfo"
slot_table = unreal.load_asset(slot_table_path)
if slot_table:
    rows = slot_table.get_row_names()
    unreal.log(f"SlotTable rows ({len(rows)}): {list(rows)[:5]}...")
else:
    unreal.log_warning(f"SlotTable not found at {slot_table_path}")

unreal.log("Configuration script complete")
