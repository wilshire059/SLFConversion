"""Run Dialog DataTable migration using C++ automation."""
import unreal

unreal.log_warning("Starting Dialog DataTable migration...")

# Call the C++ migration function
result = unreal.SLFAutomationLibrary.migrate_all_dialog_data_tables()
unreal.log_warning("Migration result:")
unreal.log_warning(result)
