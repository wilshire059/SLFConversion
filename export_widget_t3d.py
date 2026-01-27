"""
Export W_Settings_CategoryEntry widget as text to compare between projects.
"""
import unreal
import os

# Determine which project we're in
project_name = unreal.Paths.get_project_file_path()
is_bp_only = "bp_only" in project_name.lower()
project_label = "bp_only" if is_bp_only else "SLFConversion"

output_dir = "C:/scripts/SLFConversion/migration_cache/settings_comparison"
os.makedirs(output_dir, exist_ok=True)

# Export the category entry widget
entry_path = "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_CategoryEntry"
output_file = os.path.join(output_dir, f"W_Settings_CategoryEntry_{project_label}.txt")

# Use export to text
result = unreal.AssetExportTask()
result.set_editor_property('automated', True)
result.set_editor_property('prompt', False)
result.set_editor_property('filename', output_file)
result.set_editor_property('object', unreal.load_asset(entry_path))
result.set_editor_property('exporter', None)  # Use default exporter
result.set_editor_property('write_empty_files', False)

success = unreal.Exporter.run_asset_export_task(result)
print(f"Export result: {success}")
print(f"Output file: {output_file}")

# Also export W_Settings to see per-instance Icon values
settings_path = "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings"
settings_output_file = os.path.join(output_dir, f"W_Settings_{project_label}.txt")

result2 = unreal.AssetExportTask()
result2.set_editor_property('automated', True)
result2.set_editor_property('prompt', False)
result2.set_editor_property('filename', settings_output_file)
result2.set_editor_property('object', unreal.load_asset(settings_path))
result2.set_editor_property('exporter', None)
result2.set_editor_property('write_empty_files', False)

success2 = unreal.Exporter.run_asset_export_task(result2)
print(f"W_Settings Export result: {success2}")
print(f"W_Settings Output file: {settings_output_file}")
