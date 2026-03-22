"""
Apply and verify Settings widget data using C++ Automation
Run after migration is complete to update/verify all properties
"""
import unreal

CACHE_FILE = "C:/scripts/SLFConversion/migration_cache/settings_data.json"

unreal.log_warning("=== APPLYING AND VERIFYING SETTINGS DATA ===")

# Apply data
unreal.log_warning("\n--- APPLYING DATA ---")
apply_result = unreal.SLFAutomationLibrary.apply_settings_widget_data(CACHE_FILE)
unreal.log_warning(apply_result)

# Verify data
unreal.log_warning("\n--- VERIFYING DATA ---")
verify_result = unreal.SLFAutomationLibrary.verify_settings_widget_data(CACHE_FILE)
unreal.log_warning(verify_result)

# Final diagnosis
unreal.log_warning("\n--- FINAL DIAGNOSIS ---")
diag_result = unreal.SLFAutomationLibrary.diagnose_settings_widgets()
unreal.log_warning(diag_result)

unreal.log_warning("=== COMPLETE ===")
