"""
Run C++ ExtractEmbeddedSettingsWidgets on current project
"""
import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/settings_embedded_current.json"

result = unreal.SLFAutomationLibrary.extract_embedded_settings_widgets(OUTPUT_FILE)
unreal.log_warning(f"Result: {result}")
