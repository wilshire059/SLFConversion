# extract_equipment_data.py
# Extracts ItemClass and AttachmentSockets from PDA_Item data assets
# Run on bp_only project BEFORE migration to capture original values

import unreal

output_path = "C:/scripts/SLFConversion/migration_cache/equipment_data.json"

result = unreal.SLFAutomationLibrary.extract_item_equipment_data(output_path)
print(f"Extracted equipment data for {result} items to {output_path}")
