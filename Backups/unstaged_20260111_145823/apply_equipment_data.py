# apply_equipment_data.py
# Applies ItemClass and AttachmentSockets from JSON cache to PDA_Item data assets
# Run on SLFConversion project AFTER migration to restore equipment data

import unreal

input_path = "C:/scripts/SLFConversion/migration_cache/equipment_data.json"

result = unreal.SLFAutomationLibrary.apply_item_equipment_data(input_path)
print(f"Applied equipment data to {result} items from {input_path}")
