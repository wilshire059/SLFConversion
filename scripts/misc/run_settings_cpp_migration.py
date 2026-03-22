"""
Run Settings Widget Migration using C++ Automation on SLFConversion
"""
import unreal

CACHE_FILE = "C:/scripts/SLFConversion/migration_cache/settings_data.json"

unreal.log_warning("=== SETTINGS MIGRATION (SLFConversion C++) ===")

# First diagnose current state
unreal.log_warning("\n--- DIAGNOSIS BEFORE MIGRATION ---")
diag_result = unreal.SLFAutomationLibrary.diagnose_settings_widgets()
unreal.log_warning(diag_result)

# Run full migration
unreal.log_warning("\n--- RUNNING FULL MIGRATION ---")
migrate_result = unreal.SLFAutomationLibrary.migrate_settings_widgets(CACHE_FILE)
unreal.log_warning(migrate_result)

# Final diagnosis
unreal.log_warning("\n--- DIAGNOSIS AFTER MIGRATION ---")
diag_result2 = unreal.SLFAutomationLibrary.diagnose_settings_widgets()
unreal.log_warning(diag_result2)

unreal.log_warning("=== MIGRATION COMPLETE ===")
