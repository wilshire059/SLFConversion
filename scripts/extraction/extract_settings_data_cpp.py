"""
Extract settings widget data using C++ SLFAutomationLibrary.
Run on bp_only project to get original data.
"""
import unreal

OUTPUT_PATH = "C:/scripts/SLFConversion/migration_cache/settings_data_bponly.json"

unreal.log_warning("=== EXTRACTING SETTINGS DATA USING C++ ===")

result = unreal.SLFAutomationLibrary.extract_settings_widget_data(OUTPUT_PATH)
unreal.log_warning(result)

unreal.log_warning(f"=== EXTRACTION COMPLETE: {OUTPUT_PATH} ===")
