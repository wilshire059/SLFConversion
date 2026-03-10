"""Run Dialog DataTable verification using C++ automation."""
import unreal

unreal.log_warning("Starting Dialog DataTable verification...")

# Call the C++ verification function
result = unreal.SLFAutomationLibrary.verify_dialog_data_tables()
unreal.log_warning(result)
