"""
Export NPC System YAML documentation from bp_only project
This creates the source of truth for validation
"""
import unreal

output_folder = "C:/scripts/SLFConversion/NPC_System_Documentation"
count = unreal.SLFYAMLExporter.export_npc_system_to_yaml(output_folder)
print(f"[bp_only] Exported {count} NPC system assets to YAML")
print(f"Output folder: {output_folder}")
